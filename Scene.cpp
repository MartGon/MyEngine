#include "Scene.h"
#include "Navigator.h"
#include <stdio.h>

// Attributes
Uint16 Scene::lastGameObjectID = 0;

// Constructors

Scene::Scene()
{
	renderer = RendererManager::renderer;
}

Scene::Scene(Scene::SceneMode mode) : Scene()
{
	setSceneMode(mode);
}

Scene::Scene(SDL_Renderer* renderer) : Scene()
{

}

Scene::~Scene()
{
}

// Virtual Methods

void Scene::loadMedia() 
{
	printf("Base class\n");
}

void Scene::start()
{

}

void Scene::beforeUpdate()
{

}

void Scene::onUpdate()
{

}

void Scene::destroy()
{
	//printf("Destroying scene\n");
	for (auto &gameObject : gameObjectsToInitialize)
		gameObject->destroy();

	for (auto &gameObjectPair : gameObjectMap)
		if (GameObject *gameObject = gameObjectPair.second)
			gameObject->destroy();

	// Reset las id
	lastGameObjectID = 0;

	// Destroy network agent
	destroyNetworkAgent();

	this->~Scene();
}

// TODO - OnHandleEvent
void Scene::handleEvent(const SDL_Event& event) 
{
	OnHandleEvent(event);

	for (auto gameObjectPair : gameObjectMap)
	{
		GameObject* go = gameObjectPair.second;

		if (!go->shouldBeLoaded())
			continue;

		if (isOnline() && !shouldSendGameObjectUpdate(go))
			continue;

		if (go->handleEvent(event))
			return;
	}
}

// Methods

void Scene::addGameObject(GameObject *gameObject)
{
	gameObjectsToInitialize.push_back(gameObject);
}

void Scene::removeGameObject(GameObject *gameObject)
{
	gameObjectMap.erase(gameObject->id);
}

void Scene::initGameObject(GameObject *gameObject)
{
	gameObject->start();

	// Send notification of a created gameObject
	if (isOnline() && connectionEstablished)
		if(!gameObject->netCreated)
		{
			Packet* packet = new GameObjectCreatePacket(gameObject);
			networkAgent->sendPacket(packet);
			delete packet;
		}

	gameObjectsToInitialize.erase(gameObjectsToInitialize.begin());
	gameObjectMap.insert_or_assign(gameObject->id, gameObject);
}

void Scene::destroyGameObject(GameObject* gameObject)
{
	gameObjectsToDestroy.push_back(gameObject);
}

GameObject* Scene::getGameObjectById(int id)
{
	if (gameObjectMap.find(id) == gameObjectMap.end())
	{
		//std::cout << "Packet id was not valid\n";
		return nullptr;
	}
	GameObject *gameObject = gameObjectMap.at(id);

	return gameObject;
}

void Scene::update()
{
	// Return if paused
	if (isPaused)
		return;

	// Destroy objects that were set to be destroyed
	while (!gameObjectsToDestroy.empty())
	{
		GameObject* go = gameObjectsToDestroy.front();
		gameObjectsToDestroy.erase(gameObjectsToDestroy.begin());
		removeGameObject(go);
		delete go;
	}

	// Handle received packets at the start of this frame
	if (isOnline() && connectionEstablished)
	{
		// Critical Section start
		SDL_SemWait(sem);

		// Clone safely to a second vector
		std::vector<Packet*> packets_to_read = recv_packets;

		// Clear the original vector
		recv_packets.clear();

		SDL_SemPost(sem);
		// Critical Section End

		// Handle each packet before updating the objects
		for (auto packet : packets_to_read)
		{
			handlePacket(packet);
			delete packet;
		}
	}

	// Init gameObjects
	// This loops allows the list to be altered during the gameObjects' initialization
	while (!gameObjectsToInitialize.empty())
	{
		initGameObject(gameObjectsToInitialize.front());
	}

	if (isOnline())
	{
		// Try to establish connection
		if (!connectionEstablished)
		{
			if (connectionEstablished = networkAgent->establishConnection())
				handleConnectionEstablished();
			return;
		}
	}

	// Update every object
	for (auto &gameObjectPair : gameObjectMap)
	{
		if (GameObject *gameObject = gameObjectPair.second)
		{
			if (gameObject->shouldBeLoaded())
			{
				// Update the object
				gameObject->update();
			}
		}
	}

	// Update Managers
	for (auto manager : managers)
		manager->manage();

	// Update hook
	onUpdate();

	// Send notification of updated gameobjects
	//if(frame_count % 2 == 0)
	for (auto &gameObjectPair : gameObjectMap)
	{
		if (GameObject *gameObject = gameObjectPair.second)
		{
			if (isOnline() && shouldSendGameObjectUpdate(gameObject))	// Once connection is established
				sendGameObjectUpdate(gameObject);
		}
	}

	//++frame_count;
}

void Scene::deactivateAllGameObjects()
{
	for (auto &gameObjectPair : gameObjectMap)
		if (GameObject *gameObject = gameObjectPair.second)
			gameObject->isActive = false;
}

void Scene::activateAllGameObjects()
{
	for (auto &gameObjectPair : gameObjectMap)
		if (GameObject *gameObject = gameObjectPair.second)
			gameObject->isActive = true;
}

// Network

void Scene::destroyNetworkAgent()
{
	if (alreadyDestroyed)
		return;

	if (isOnline())
	{
		if (sem)
		{
			SDL_DestroySemaphore(sem);
			sem = nullptr;
		}

		networkAgent->destroy();

		alreadyDestroyed = true;
	}
}

void Scene::setSceneMode(Scene::SceneMode sceneMode)
{
	printf("Setting scene mode\n");

	mode = sceneMode;

	switch (sceneMode)
	{
	case Scene::ONLINE_CLIENT:
		networkAgent = new NetworkClient();
		break;
	case Scene::ONLINE_SERVER:
		networkAgent = new NetworkServer();
		break;
	}

	if (isOnline())
	{
		// Threading
		sem = SDL_CreateSemaphore(1);
		thread = SDL_CreateThread(recvPacketThread, "defu", this);
		SDL_DetachThread(thread);
	}
}

bool Scene::isOnline()
{
	return mode == ONLINE_CLIENT || mode == ONLINE_SERVER;
}

bool Scene::shouldSendGameObjectUpdate(GameObject* go)
{
	if (go->isNetworkUpdated())
	{
		if (mode == ONLINE_SERVER)
		{
			if (!go->shouldBeUpdatedFromClient())
				return true;
		}
		else if (mode == ONLINE_CLIENT)
		{
			if (go->shouldBeUpdatedFromClient())
				return true;
		}
	}

	return false;
}

void Scene::sendGameObjectUpdate(GameObject* go)
{
	// GameObject State
	Packet* packet = go->toGameObjectUpdatePacket();
	networkAgent->sendPacket(packet);
	delete packet;

	// Transform
	packet = go->transform.toComponentPacket();
	networkAgent->sendPacket(packet);
	delete packet;

	for (Component* component : go->components)
	{
		Packet* packet = component->toComponentPacket();
		if(packet->packetType != PacketType::NULL_PACKET)
			networkAgent->sendPacket(packet);
		delete packet;
	}

	return;
}

void Scene::disconnect()
{
	onDisconnect();

	disconnected = true;
}

bool Scene::handlePacket(Packet *packet)
{
	if (!packet)
		return false;

	switch (packet->packetType)
	{
		case PacketType::COMPONENT_PACKET:
		{
			ComponentPacket* component_packet = static_cast<ComponentPacket*>(packet);

			// Get gameobject
			int id = component_packet->gameobject_id;

			if (GameObject* gameObject = getGameObjectById(id))
			{
				if (!shouldSendGameObjectUpdate(gameObject))
					gameObject->updateGameObjectFromComponentPacket(component_packet);
			}
			break;
		}
		case PacketType::GAMEOBJECT_UPDATE_PACKET:
		{
			GameObjectUpdatePacket* gameobject_update_packet = static_cast<GameObjectUpdatePacket*>(packet);

			// Get gameobject
			int id = gameobject_update_packet->gameobject_id;

			if (GameObject* gameObject = getGameObjectById(id))
			{
				gameObject->updateFromGameObjectUpdatePacket(gameobject_update_packet);
			}
			break;
		}
		case PacketType::GAMEOBJECT_CREATE_PACKET:
		{
			// Create Packet
			GameObjectCreatePacket* gameobject_create_packet = static_cast<GameObjectCreatePacket*>(packet);

			// Create the GameObject
			int template_id = gameobject_create_packet->gameobject_template_id;
			if (GameObject* go = createGameObjectByTemplateId(template_id))
			{
				// Set flag
				go->netCreated = true;

				// Error check
				int desired_id = gameobject_create_packet->gameobject_id;
				if (go->id != desired_id)
				{
					std::cout << "Id desync issue!!\n";
					go->id = desired_id;
				}
			}
			break;
		}
		case PacketType::NULL_PACKET:
			std::cout << "Null packet"; 
			break;
		default:
			break;
	}

	return true;
}

int recvPacketThread(void* data)
{
	Scene *scene = (Scene*)data;
	Packet *packet = nullptr;
	bool valid = false;

	while (true)
	{
		if (scene->disconnected)
			break;

		if (!scene->connectionEstablished)
			continue;

		// Receive data
		packet = scene->networkAgent->recvPacket();

		/* On this critical section we are only modifying this vars
		   scene->recv_packets
		*/

		// Critical Section Start
		SDL_SemWait(scene->sem);

		if (!packet)
		{
			scene->disconnect();
			SDL_SemPost(scene->sem);
			break;
		}

		// Add to the list of packets to be read
		scene->recv_packets.push_back(packet);

		SDL_SemPost(scene->sem);
		// Critical Section End
	}

	return 0;
}