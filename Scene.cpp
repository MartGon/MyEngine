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


void Scene::handleEvent(const SDL_Event& event, bool from_network)
{
	OnHandleEvent(event);

	for (auto gameObjectPair : gameObjectMap)
	{
		GameObject* go = gameObjectPair.second;

		// Go is not active
		if (!go->shouldBeLoaded())
			continue;

		// Go is updated by pair
		if (isOnline())
		{ 
			if (from_network)
			{
				if (go->isNetworkStatic)
					continue;

				if (mode == ONLINE_CLIENT && !go->updateFromClient)
				{
					if (go->handleEvent(event))
						return;
				}
				else if (mode == ONLINE_SERVER && go->updateFromClient)
				{
					if (go->handleEvent(event))
						return;
				}
				else
					continue;
			}
			else
			{
				if (shouldSendGameObjectUpdate(go))
				{
					if (go->handleEvent(event))
						return;
				}
				else
					continue;
			}
		}
			
		// Handle local event
		if (go->handleEvent(event))
			return;
	}

	return;
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

	// Sends catched events and check for incoming ones
	if (isOnline() && connectionEstablished)
	{
		// Calculate frame diff
		InputStatusPacket* prev_packet = nullptr;

		if(!recv_packets.empty())
			prev_packet = recv_packets.front();

		int diff = 0;
		int last_packet_frame = prev_packet ? prev_packet->frame : 0;
		diff = frame_count - last_packet_frame;

		// Send packet if the delay is lesser than 10 frames
		if (diff < 2)
		{
			// We craft a Input Status Packet if we have images left
			InputStatusPacket* packet = new InputStatusPacket();

			// Get key state
			while (!to_send_events.empty())
			{
				// Get and remove event
				SDL_Event e = to_send_events.back();
				to_send_events.pop_back();

				if (e.type == SDL_KEYDOWN)
				{
					if (e.key.keysym.sym == SDLK_w)
						packet->input_flags += InputFlags::W_KEY_PRESSED;
					if (e.key.keysym.sym == SDLK_s)
						packet->input_flags += InputFlags::S_KEY_PRESSED;
					if (e.key.keysym.sym == SDLK_d)
						packet->input_flags += InputFlags::D_KEY_PRESSED;
					if (e.key.keysym.sym == SDLK_a)
						packet->input_flags += InputFlags::A_KEY_PRESSED;
				}
				else if (e.type == SDL_KEYUP)
				{
					if (e.key.keysym.sym == SDLK_w)
						packet->input_flags += InputFlags::W_KEY_RELEASED;
					if (e.key.keysym.sym == SDLK_s)
						packet->input_flags += InputFlags::S_KEY_RELEASED;
					if (e.key.keysym.sym == SDLK_d)
						packet->input_flags += InputFlags::D_KEY_RELEASED;
					if (e.key.keysym.sym == SDLK_a)
						packet->input_flags += InputFlags::A_KEY_RELEASED;
				}
				else if (e.type == SDL_MOUSEBUTTONDOWN)
				{
					if (e.button.button == SDL_BUTTON_LEFT)
						packet->input_flags += InputFlags::LEFT_MOUSE_KEY_PRESSED;
				}
				else if (e.type == SDL_MOUSEBUTTONUP)
				{
					if (e.button.button == SDL_BUTTON_LEFT)
						packet->input_flags += InputFlags::LEFT_MOUSE_KEY_RELEASED;
				}
			}

			// Get Mouse state
			int x, y;
			SDL_GetMouseState(&x, &y);
			Vector2<Uint16> mouse_pos = Vector2<Uint16>(x, y);
			packet->mouse_pos = mouse_pos;

			// Set frame number
			packet->frame = frame_count++;

			// Send Packet
			networkAgent->sendPacket(packet, false);
			delete packet;
		}

		// Are there any packets to recv
		if (Packet* recv_packet = networkAgent->recvPacket())
		{
			if (recv_packet->packetType == PacketType::INPUT_STATUS_PACKET)
			{
				InputStatusPacket* input_packet = static_cast<InputStatusPacket*>(recv_packet);
				recv_packets.push_front(input_packet);

				Uint16 flags = input_packet->input_flags;
				this->pair_mouse_state = input_packet->mouse_pos;

				// Handle inputs
				std::vector<SDL_Event> events;
				SDL_Event e;
				if (flags & InputFlags::A_KEY_PRESSED)
				{
					e.type = SDL_KEYDOWN;
					e.key.keysym.sym = SDLK_a;
					events.push_back(e);
				}
				if (flags & InputFlags::D_KEY_PRESSED)
				{
					e.type = SDL_KEYDOWN;
					e.key.keysym.sym = SDLK_d;
					events.push_back(e);
				}
				if (flags & InputFlags::S_KEY_PRESSED)
				{
					e.type = SDL_KEYDOWN;
					e.key.keysym.sym = SDLK_s;
					events.push_back(e);
				}
				if (flags & InputFlags::W_KEY_PRESSED)
				{
					e.type = SDL_KEYDOWN;
					e.key.keysym.sym = SDLK_w;
					events.push_back(e);
				}
				if (flags & InputFlags::A_KEY_RELEASED)
				{
					e.type = SDL_KEYUP;
					e.key.keysym.sym = SDLK_a;
					events.push_back(e);
				}
				if (flags & InputFlags::D_KEY_RELEASED)
				{
					e.type = SDL_KEYUP;
					e.key.keysym.sym = SDLK_d;
					events.push_back(e);
				}
				if (flags & InputFlags::S_KEY_RELEASED)
				{
					e.type = SDL_KEYUP;
					e.key.keysym.sym = SDLK_s;
					events.push_back(e);
				}
				if (flags & InputFlags::W_KEY_RELEASED)
				{
					e.type = SDL_KEYUP;
					e.key.keysym.sym = SDLK_w;
					events.push_back(e);
				}
				if (flags & InputFlags::LEFT_MOUSE_KEY_PRESSED)
				{
					e.type = SDL_MOUSEBUTTONDOWN;
					e.button.button = SDL_BUTTON_LEFT;
					events.push_back(e);
				}
				if (flags & InputFlags::LEFT_MOUSE_KEY_RELEASED)
				{
					e.type = SDL_MOUSEBUTTONUP;
					e.button.button = SDL_BUTTON_LEFT;
					events.push_back(e);
				}

				for (auto e : events)
					handleEvent(e, true);
			}
			else
			{
				std::cout << "Packet was not of type InputStatusPacket";
			}
		}
		else
		{
			std::cout << "There were no packets to recv \n";
			return;
		}
	}

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

	// Handle events
	handle_events(event_deque);

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
			pair_mouse_state = mouse_state_packet->position;
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