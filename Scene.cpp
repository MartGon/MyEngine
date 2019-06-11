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
	Random::setSeed(std::random_device()());
}

void Scene::onUpdate()
{

}

void Scene::destroy()
{
	for (auto manager : managers)
		manager->destroy();

	//printf("Destroying scene\n");
	while(!gameObjectsToInitialize.empty())
	{
		GameObject* gameObject = *gameObjectsToInitialize.begin();
		destroyGameObject(gameObject);
	}

	while (!gameObjectMap.empty())
	{
		if (GameObject *gameObject = gameObjectMap.begin()->second)
			destroyGameObject(gameObject);
	}

	// Reset las id
	lastGameObjectID = 0;

	// Destroy network agent
	destroyNetworkAgent();

	this->~Scene();
}


// GameObject Methods

void Scene::addGameObject(GameObject *gameObject)
{
	gameObjectsToInitialize.push_back(gameObject);
}

void Scene::removeGameObject(GameObject *gameObject)
{
	if(gameObjectMap.find(gameObject->id) != gameObjectMap.end())
		gameObjectMap.erase(gameObject->id);
}

void Scene::initGameObject(GameObject *gameObject)
{
	// Call start hook
	gameObject->start();

	// Remove from objects to init and add into gos map
	gameObjectsToInitialize.erase(gameObjectsToInitialize.begin());
	gameObjectMap.insert_or_assign(gameObject->id, gameObject);
}

void Scene::setGameObjectToDestroy(GameObject* gameObject)
{
	if(std::find(gameObjectsToDestroy.begin(), gameObjectsToDestroy.end(), gameObject) == gameObjectsToDestroy.end())
		gameObjectsToDestroy.push_back(gameObject);
}

void Scene::destroyGameObject(GameObject* go)
{
	// Remove from every list. Update, Start and Destroy
	removeGameObject(go);
	Utilities::removeFromVector(go, gameObjectsToInitialize);
	if(std::find(gameObjectsToDestroy.begin(), gameObjectsToDestroy.end(), go) != gameObjectsToDestroy.end())
		gameObjectsToDestroy.erase(gameObjectsToDestroy.begin());	

	// Call destructor
	delete go;
}

GameObject* Scene::getGameObjectById(int id)
{
	if (gameObjectMap.find(id) == gameObjectMap.end())
		return nullptr;

	GameObject *gameObject = gameObjectMap.at(id);

	return gameObject;
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

	inputManager->default_owner = getNetworkOwnership();
}

bool Scene::isOnline()
{
	return mode == ONLINE_CLIENT || mode == ONLINE_SERVER;
}

void Scene::disconnect()
{
	onDisconnect();

	disconnected = true;
}

NetworkOwner Scene::getNetworkOwnership()
{
	if (mode == SceneMode::ONLINE_CLIENT)
		return NetworkOwner::OWNER_CLIENT_1;
	else
		return NetworkOwner::OWNER_SERVER;
}

// Main

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

void Scene::update()
{
	// Return if paused
	if (isPaused)
		return;

	// Init gameObjects
	// This loops allows the list to be altered during the gameObjects' initialization
	while (!gameObjectsToInitialize.empty())
	{
		initGameObject(gameObjectsToInitialize.front());
	}

	// Destroy objects that were set to be destroyed
	while (!gameObjectsToDestroy.empty())
	{
		GameObject* go = gameObjectsToDestroy.front();
		gameObjectsToDestroy.erase(gameObjectsToDestroy.begin());
		removeGameObject(go);
		Utilities::removeFromVector(go, gameObjectsToInitialize);
		delete go;
	}

	// Establish connection
	if (isOnline())
	{
		// Try to establish connection
		if (!connectionEstablished)
		{
			if (connectionEstablished = networkAgent->establishConnection())
			{
				if (networkAgent)
					networkAgent->isBlocking = false;

				handleConnectionEstablished();
			}
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
		bool update_frame = false;

		// Check if we can send the next packet
		if (!stop_sending)
		{
			// Get Input Status
			InputStatus input_status = inputManager->getInputStatus(getNetworkOwnership());

			// Add inputstatus to history
			input_status_history.insert({ frame_count, input_status });

			// Craft Packet to send
			InputStatusPacket* packet = new InputStatusPacket(frame_count, input_status.input_flags);
			packet->mouse_pos = input_status.mouse_pos;

			// Send Packet
			networkAgent->sendPacket(packet, false);
			delete packet;

			// Increment frame_count
			frame_count++;
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

				// Revert previously sent input status
				if (input_status_history.find(input_packet->frame_count) != input_status_history.end())
				{
					InputStatus prev_input_status = input_status_history.at(input_packet->frame_count);
					inputManager->setInputStatus(prev_input_status, getNetworkOwnership());
				}

				// Revert also one frame before if not the first frame
				int prev_frame = last_packet ? last_packet->frame_count : 0;
				if (input_status_history.find(prev_frame) != input_status_history.end())
				{
					InputStatus prev_bis_input_status = input_status_history.at(prev_frame);
					inputManager->setPrevInputStatus(prev_bis_input_status, getNetworkOwnership());
				}

				// Set last_packet
				last_packet = input_packet;

				// Set flag
				update_frame = true;
			}

			// Check if we can start sending packets again
			if (stop_sending)
			{
				if (RendererManager* renderer_manager = static_cast<RendererManager*>(getManager<TextureRenderer*>()))
				{
					stop_sending = false;
				}
			}
		}
		// No packet was received
		else
		{
			std::cout << "There were no packets to recv \n";

			// Check how many frames we are off
			if (!stop_sending)
			{
				if (RendererManager* renderer_manager = static_cast<RendererManager*>(getManager<TextureRenderer*>()))
				{
					if (last_packet)
					{
						// If last recv packet is 10 frames old, wait for sync
						if (std::abs((int)(frame_count - last_packet->frame_count)) >= renderer_manager->getMaxBufferSize())
						{
							stop_sending = true;
							std::cout << "Timeout Ocurred, need to ReSync\n";
						}
					}
				}
			}
		}

		if (!update_frame)
			return;
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

	// FrameCount for offline
	if (!isOnline())
		frame_count++;
}