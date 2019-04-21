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
		gameObjectPair.second->handleEvent(event);
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

	gameObjectsToInitialize.erase(gameObjectsToInitialize.begin());
	gameObjectMap.insert_or_assign(gameObject->id, gameObject);
}

void Scene::destroyGameObject(GameObject* gameObject)
{
	gameObjectsToDestroy.push_back(gameObject);
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
		initGameObject(gameObjectsToInitialize.front());

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

                // Online measures
                if (isOnline())
                    // Once connection is established
                {
                    if (mode == ONLINE_SERVER)
                    {
						if (Transform* transform = &gameObject->transform)
						{
							TransformPacket transform_packet = TransformPacket(transform);
							networkAgent->sendPacket(&transform_packet);
						}
						if (Navigator* nav = gameObject->getComponent<Navigator>())
						{
							if (nav->isEnabled)
							{
								NavigatorPacket* nav_packet = new NavigatorPacket(nav);
								networkAgent->sendPacket(nav_packet);
								delete nav_packet;
							}
						}
                    }
					/*
                    else if (mode == ONLINE_CLIENT)
                    {
                        if (gameObject->updateFromClient)
                        {
							if (Transform* transform = &gameObject->transform)
							{
								TransformPacket transform_packet = TransformPacket(transform);
								networkAgent->sendPacket(&transform_packet);
							}
							if (Navigator* nav = gameObject->getComponent<Navigator>())
							{
								if (nav->isEnabled)
								{
									NavigatorPacket* nav_packet = new NavigatorPacket(nav);
									networkAgent->sendPacket(nav_packet);
									delete nav_packet;
								}
							}
                        }
                    }
					*/
                }
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
			if (gameObjectMap.find(id) == gameObjectMap.end())
			{
				//std::cout << "Packet id was not valid\n";
				return true;
			}
			GameObject *gameObject = gameObjectMap.at(id);

			if (mode == ONLINE_SERVER)
			{
				if (gameObject->updateFromClient)
				{
					if (component_packet->sub_type == ComponentPacketType::COMPONENT_NAVIGATOR)
					{
						NavigatorPacket* navigator_packet = static_cast<NavigatorPacket*>(component_packet);
						if (Navigator *nav = gameObject->getComponent<Navigator>())
						{
							nav->speed = navigator_packet->speed;
							nav->setDirection(navigator_packet->direction);
							nav->acceleration = navigator_packet->acceleration;
							nav->isKinematic = navigator_packet->isKinematic;
							nav->stopAtInflectionPoint = navigator_packet->stopAtInflectionPoint;
						}
					}
					else if (component_packet->sub_type == ComponentPacketType::COMPONENT_TRANSFORM)
					{
						TransformPacket* transform_packet = static_cast<TransformPacket*>(component_packet);
						if (Transform* transform = &gameObject->transform)
						{
							// Set parent
							if (transform_packet->parent_id == -1)
								transform->parent = nullptr;
							else if (gameObjectMap.find(transform_packet->parent_id) == gameObjectMap.end())
							{
								Transform *parent = &gameObjectMap.at(id)->transform;
								transform->parent = parent;
							}

							transform->position = transform_packet->position;
							transform->zRotation = transform_packet->zRotation;
							transform->scale = transform_packet->scale;
							if (transform->rotationCenter)
								*transform->rotationCenter = transform_packet->rotationCenter;
						}
					}
				}
			}
			else if (mode == ONLINE_CLIENT)
			{
				if (component_packet->sub_type == ComponentPacketType::COMPONENT_NAVIGATOR)
				{
					NavigatorPacket* navigator_packet = static_cast<NavigatorPacket*>(component_packet);
					if (Navigator *nav = gameObject->getComponent<Navigator>())
					{
						nav->speed = navigator_packet->speed;
						nav->setDirection(navigator_packet->direction);
						nav->acceleration = navigator_packet->acceleration;
						nav->isKinematic = navigator_packet->isKinematic;
						nav->stopAtInflectionPoint = navigator_packet->stopAtInflectionPoint;
					}
				}
				else if (component_packet->sub_type == ComponentPacketType::COMPONENT_TRANSFORM)
				{
					TransformPacket* transform_packet = static_cast<TransformPacket*>(component_packet);
					if (Transform* transform = &gameObject->transform)
					{
						// Set parent
						if (transform_packet->parent_id == -1)
							transform->parent = nullptr;
						else if (gameObjectMap.find(transform_packet->parent_id) == gameObjectMap.end())
						{
							Transform *parent = &gameObjectMap.at(id)->transform;
							transform->parent = parent;
						}

						transform->position = transform_packet->position;
						transform->zRotation = transform_packet->zRotation;
						transform->scale = transform_packet->scale;
						if (transform->rotationCenter)
							*transform->rotationCenter = transform_packet->rotationCenter;
					}
				}
			}

			break;
		}
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