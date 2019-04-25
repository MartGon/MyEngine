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
	for (auto gameObjectPair : gameObjectMap)
	{
		GameObject* go = gameObjectPair.second;
		if (isOnline() && !shouldSendGameObjectUpdate(go))
			go->handleEvent(event);
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

	if (isOnline() && connectionEstablished)
		if(!gameObject->netCreated)
		{
			if (gameObject->template_id == 1)
				std::cout << "Sending arrow \n";

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
				if(isOnline())
					SDL_SemWait(sem);

                // Update the object
                gameObject->update();

				if(isOnline())
					SDL_SemPost(sem);
            }

			// Online measures
			if (isOnline() && shouldSendGameObjectUpdate(gameObject))	// Once connection is established
				sendGameObjectUpdate(gameObject);
        }
    }

	// Update Managers
	for (auto manager : managers)
		manager->manage();

	// Update hook
	onUpdate();
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

		SDL_SemWait(scene->sem);

		if (!packet)
		{
			scene->disconnect();
			SDL_SemPost(scene->sem);
			break;
		}

		valid = scene->handlePacket(packet);

		if (!valid)
		{
			scene->disconnect();
			SDL_SemPost(scene->sem);
			break;
		}

		SDL_SemPost(scene->sem);
	}

	return 0;
}