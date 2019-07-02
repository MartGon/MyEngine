#include "Scene.h"
#include "Navigator.h"
#include "Random.h"
#include "TextLabel.h"
#include <stdio.h>

// Attributes
Uint16 Scene::lastGameObjectID = 0;
SDL_Texture* Scene::frame_to_render = nullptr;
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
	Random::setSeed(Random::seed);

	if (isOnline())
	{
		// Create online log
		std::string client_type = mode == ONLINE_CLIENT ? "client" : "server";
		std::string logfile_name = "logs/" + client_type + "_" + std::to_string(networkAgent->pair_identity) + "_" + "log";
		online_log = std::ofstream(logfile_name);
	}
}

void Scene::onUpdate()
{

}

void Scene::destroy()
{
	for (auto manager : managers)
	{
		manager->destroy();
	}

	//printf("Destroying scene\n");
	for(auto gameObject : gameObjectsToInitialize)
	{
		if (gameObject)
		{
			delete gameObject;
		}
	}

	for (auto gop : gameObjectMap)
	{
		if (GameObject *gameObject = gop.second)
		{
			delete gameObject;
		}
	}

	// Destroy objects that were set to be destroyed
	for (auto gameObject : gameObjectsToDestroy)
	{
		delete gameObject;
	}

	for (auto manager : managers)
	{
		delete manager;
	}

	delete inputManager;
	inputManager = nullptr;

	// Reset las id
	lastGameObjectID = 0;

	if (isOnline())
	{
		// Close log
		online_log.close();
	}

	// Destroy network agent
	destroyNetworkAgent();

	// Call hook
	onDestroy();

	return;
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

void Scene::addOnlineLogLine(InputStatus status, Uint32 owner, Uint32 frame)
{
	std::string log_line;

	if (online_log)
	{
		// Set frame prefix info
		log_line = "Frame:\t" + std::to_string(frame);

		// Set owner info
		log_line += "\tClient:\t" + std::to_string(owner);

		// Set inputs
		log_line += "\tW Pressed\t" + Utilities::boolToStr(status.input_flags & W_KEY_PRESSED);
		log_line += "\tS Pressed\t" + Utilities::boolToStr(status.input_flags & S_KEY_PRESSED);
		log_line += "\tD Pressed\t" + Utilities::boolToStr(status.input_flags & D_KEY_PRESSED);
		log_line += "\tA Pressed\t" + Utilities::boolToStr(status.input_flags & A_KEY_PRESSED);

		// Button
		log_line += "\tLMB Pressed\t" + Utilities::boolToStr(status.input_flags & LEFT_MOUSE_KEY_PRESSED);
		log_line += "\tRMB Pressed\t" + Utilities::boolToStr(status.input_flags & RIGHT_MOUSE_KEY_PRESSED);

		// Mouse position
		log_line += std::string("\tMousePos\t") +"(" + std::to_string(status.mouse_pos.x) + ", " + std::to_string(status.mouse_pos.y) + ")";

		// Random state
		log_line += "\tRandomState\t" + std::to_string(Random::called_counter);

		// Line break
		log_line += "\n";

		// Add to log
		online_log << log_line;
	}

	return;
}

void Scene::destroyNetworkAgent()
{
	if (networkAgent)
	{
		delete networkAgent;
		networkAgent = nullptr;
	}
}

void Scene::setSceneMode(Scene::SceneMode sceneMode)
{
	mode = sceneMode;

	switch (sceneMode)
	{
	case Scene::ONLINE_CLIENT:
		if(!networkAgent)
			networkAgent = new NetworkClient();
		break;
	case Scene::ONLINE_SERVER:
		if(!networkAgent)
			networkAgent = new NetworkServer();
		break;
	}

	inputManager->default_owner = getNetworkOwnership();
}

bool Scene::isOnline()
{
	return mode == ONLINE_CLIENT || mode == ONLINE_SERVER;
}

NetworkOwner Scene::getNetworkOwnership()
{
	NetworkOwner owner = NetworkOwner::OWNER_SERVER;
	if (mode == SceneMode::ONLINE_CLIENT)
	{
		owner = (NetworkOwner)networkAgent->pair_identity;
	}
	
	return owner;
}

bool Scene::canCalculateFrame(Uint32 frame)
{
	bool update_frame = true;
	for (int i = 0; i < networkAgent->player_amount; i++)
	{
		// Get input history for pair i
		NetworkOwner o = (NetworkOwner)i;
		InputHistory input_history = input_histories.at(o);

		// Check if packet exists
		if (input_history.find(frame) == input_history.end())
		{
			// Cannot calculate this frame if a packet is missing
			update_frame = false;
			break;
		}
	}

	return update_frame;
}

InputStatusPacket* Scene::getOldestLastPacket()
{
	InputStatusPacket* oldest_packet = nullptr;
	for (auto pair : last_packets)
	{
		InputStatusPacket* packet = pair.second;

		if (packet)
		{
			if (oldest_packet)
			{
				if (packet->frame_count < oldest_packet->frame_count)
					oldest_packet = packet;
			}
			else
				oldest_packet = packet;
		}
	}

	return oldest_packet;
}

bool Scene::shouldStopSending()
{
	bool stop_sending = false;

	for (auto pair : last_packets)
	{
		InputStatusPacket* last_packet = pair.second;

		if (last_packet)
		{
			// If last recv packet is 10 frames old, wait for sync
			if (std::abs((int)(frame_count - last_packet->frame_count)) >= networkAgent->max_buffer_size)
			{
				stop_sending = true;
				std::cout << "Timeout Ocurred, need to ReSync\n";
			}
		}
	}

	return stop_sending;
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

				// Creat input histories
				for (int i = 0; i < networkAgent->player_amount; i++)
					input_histories.insert({ (NetworkOwner)i, std::unordered_map<Uint32, InputStatus>() });

				// Create last packets
				for (int i = 0; i < networkAgent->player_amount; i++)
					last_packets.insert({ (NetworkOwner)i , nullptr });

				// Write res
				Vector2<int> win_size;
				SDL_GetWindowSize(RendererManager::window, &win_size.x, &win_size.y);
				std::string res_str = "Resolution: " + std::to_string(win_size.x) + " x " + std::to_string(win_size.y);
				online_log << res_str << "\n";

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

			// Get input history
			InputHistory input_history = input_histories.at(getNetworkOwnership());

			// Add inputstatus to history
			input_history.insert( { frame_count, input_status } );

			// Set back
			input_histories.at(getNetworkOwnership()) = input_history;

			// Craft Packet to send
			InputStatusPacket* packet = new InputStatusPacket(frame_count, input_status.input_flags);
			packet->mouse_pos = input_status.mouse_pos;
			packet->owner_identity = getNetworkOwnership();

			// Send Packet
			networkAgent->sendPacket(packet, false);
			delete packet;

			// Increment frame_count
			frame_count++;
		}

		// Wait for packets
		std::vector<Packet*> recv_packets = networkAgent->recvPackets();
		for (auto recv_packet : recv_packets)
		{
			if (recv_packet)
			{
				if (recv_packet->packetType == PacketType::INPUT_STATUS_PACKET)
				{
					// Get packet
					InputStatusPacket* input_packet = static_cast<InputStatusPacket*>(recv_packet);

					// Get pair input status
					InputStatus input_status;
					input_status.input_flags = input_packet->input_flags;
					input_status.mouse_pos = input_packet->mouse_pos;
					Uint32 frame = input_packet->frame_count;

					// Get pair identity
					NetworkOwner owner = (NetworkOwner)recv_packet->owner_identity;

					std::cout << "I recv packet from " << owner << " with frame " << frame << "\n";

					// Free last packet if it exists
					if (Packet* last_packet = last_packets.at(owner))
					{
						// Free old one
						std::free(last_packet);
					}

					// Set new packet
					last_packets.at(owner) = input_packet;

					// Inset input status recv
					InputHistory input_history = input_histories.at(owner);
					input_history.insert({ frame, input_status });
					input_histories.at(owner) = input_history;

					// If server, send packet to pairs
					if (getNetworkOwnership() == NetworkOwner::OWNER_SERVER)
					{
						InputStatusPacket re_packet = InputStatusPacket(*input_packet);
						networkAgent->sendPacket(&re_packet, false);
					}
				}
			}
			// No packet was received
			else
			{
				//std::cout << "There were no packets to recv \n";

				// Check how many frames we are off
			}
		}

		// Check if we can calculate the frame we recieved
		// Set Flag
		update_frame = canCalculateFrame(calc_frame_count);

		// Revert input_statuses to frame_checked
		if (update_frame)
		{
			std::cout << "Calculating frame " << calc_frame_count << "\n";

			for (int i = 0; i < networkAgent->player_amount; i++)
			{
				// Parse to Owner
				NetworkOwner o = (NetworkOwner)i;

				// Get input history of owner
				InputHistory i_history = input_histories.at(o);

				// Recover i_status of frame @frame
				InputStatus i_status = i_history.at(calc_frame_count);

				// Set Input status
				inputManager->setInputStatus(i_status, o);

				// Add log line
				addOnlineLogLine(i_status, i, calc_frame_count);

				// Recover prev input status of frame @frame
				Uint32 prev_frame = calc_frame_count > 0 ? calc_frame_count - 1 : 0;
				InputStatus prev_i_status = i_history.at(prev_frame);

				// Delete entry
				if (prev_frame)
					i_history.erase(prev_frame);

				// Re-insert
				input_histories.at(o) = i_history;

				// Set prev input status
				inputManager->setPrevInputStatus(prev_i_status, o);
			}

			// Update calculated frame counter
			calc_frame_count++;

			// Frame was updated, we can continue sending packets
			stop_sending = false;
		}

		if (!stop_sending)
			stop_sending = shouldStopSending();

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