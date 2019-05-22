#include "Scene.h"
#include "Navigator.h"
#include "Random.h"
#include <stdio.h>

// Attributes
Uint16 Scene::lastGameObjectID = 0;

// Constructors

Scene::Scene()
{
	renderer = RendererManager::renderer;
	inputManager = new InputManager();
	//setManager(inputManager);
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


GameObject* Scene::handleEvent(const SDL_Event& event, bool from_network)
{
	OnHandleEvent(event);

	for (auto gameObjectPair : gameObjectMap)
	{
		GameObject* go = gameObjectPair.second;

		// Go is not active
		if (!go->shouldBeLoaded())
			continue;
			
		// Handle local event
		if (go->handleEvent(event))
			return go;
	}

	return nullptr;
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
			//networkAgent->sendPacket(packet);
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

	// Establish connection
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

	// Handle Events
	handle_events(event_deque);

	// Update InputManager
	inputManager->manage();

	// Send and wait for Input Status packet
	if (isOnline() && connectionEstablished)
	{
		if (!stop_sending)
		{
			// Get Input Status
			InputStatus input_status = inputManager->getInputStatus(getNetworkOwnership());

			// Craft Packet to send
			InputStatusPacket* packet = new InputStatusPacket(frame_count, input_status.input_flags);
			packet->mouse_pos = input_status.mouse_pos;

			// Increment frame_count
			frame_count++;

			// Send Packet
			networkAgent->sendPacket(packet, false);
			delete packet;
		}

		// Wait for packet
		if (Packet* recv_packet = networkAgent->recvPacket())
		{
			if (recv_packet->packetType == PacketType::INPUT_STATUS_PACKET)
			{
				InputStatusPacket* input_packet = static_cast<InputStatusPacket*>(recv_packet);

				// Get pair input status
				InputStatus input_status;
				input_status.input_flags = input_packet->input_flags;
				input_status.mouse_pos = input_packet->mouse_pos;

				// Get pair identity
				NetworkOwner owner = mode == ONLINE_SERVER ? NetworkOwner::OWNER_CLIENT_1 : NetworkOwner::OWNER_SERVER;

				// Add to input status
				inputManager->setInputStatus(input_status, owner);

				// Set last_packet
				last_packet = input_packet;
			}
			else
			{
				std::cout << "Packet was not of type InputStatusPacket";
			}

			if (stop_sending)
			{
				if (RendererManager* renderer_manager = static_cast<RendererManager*>(getManager<TextureRenderer*>()))
				{
					if (std::abs((int)(frame_count - last_packet->frame_count)) == 0)
					{
						while(renderer_manager->frame_buffer.size() != renderer_manager->getMaxBufferSize())
						{
							renderer_manager->manage();
						}
						stop_sending = false;
					}
				}
			}
		}
		else
		{
			//std::cout << "There were no packets to recv \n";

			// Check if we can send next packet
			if (!stop_sending)
			{
				if (RendererManager* renderer_manager = static_cast<RendererManager*>(getManager<TextureRenderer*>()))
				{
					if (last_packet)
					{
						// If last recv packet is 10 frames old, wait for sync
						if (std::abs((int)(frame_count - last_packet->frame_count)) >= renderer_manager->frame_buffer.size())
						{
							stop_sending = true;
							std::cout << "Timeout Ocurred, need to Desync\n";
						}
					}
				}
			}
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

	if (networkAgent)
		networkAgent->isBlocking = false;
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
			if (!go->isNetworkOwned())
				return true;
		}
		else if (mode == ONLINE_CLIENT)
		{
			if (go->isNetworkOwned())
				return true;
		}
	}

	return false;
}

void Scene::sendGameObjectUpdate(GameObject* go)
{
	// GameObject State
	
	Packet* packet = go->toGameObjectUpdatePacket();
	networkAgent->sendPacket(packet, false);
	delete packet;

	// Transform
	packet = go->transform.toComponentPacket();
	networkAgent->sendPacket(packet, false);
	delete packet;

	for (Component* component : go->components)
	{
		Packet* packet = component->toComponentPacket();
		if(packet->packetType != PacketType::NULL_PACKET)
			networkAgent->sendPacket(packet, false);
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
		case PacketType::EVENT_PACKET:
		{
			/*
			EventPacket* event_packet = static_cast<EventPacket*>(packet);
			if(GameObject* go = getGameObjectById(event_packet->gameobject_id))
				go->handleEvent(event_packet->event);
			break;
			*/
		}
		case PacketType::MOUSE_STATE_PACKET:
		{
			MouseStatePacket* mouse_state_packet = static_cast<MouseStatePacket*>(packet);
			//pair_mouse_state = mouse_state_packet->position;
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

void Scene::handle_events(std::deque<SDL_Event>& events, bool network)
{
	while (!event_deque.empty())
	{
		// Get and remove event
		SDL_Event event = events.back();
		events.pop_back();

		handleEvent(event);
	}
}

NetworkOwner Scene::getNetworkOwnership()
{
	if (mode == SceneMode::ONLINE_CLIENT)
		return NetworkOwner::OWNER_CLIENT_1;
	else
		return NetworkOwner::OWNER_SERVER;
}