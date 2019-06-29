#pragma once
#include "GameObject.h"
#include "RendererManager.h"
#include "CollisionManager.h"
#include "NetworkServer.h"
#include "NetworkClient.h"
#include "Monitor.h"
#include "InputManager.h"
#include <SDL_thread.h>
#include <SDL.h>
#include <vector>
#include <deque>
#include <map>
#include <chrono>
#include <fstream>

class Scene
{
public:

	// Scene modes
	enum SceneMode 
	{
		SINGLE_PLAYER,
		LOCAL_MULTIPLAYER,
		ONLINE_CLIENT,
		ONLINE_SERVER
	};

	// Constructors
	Scene();
	Scene(SDL_Renderer* renderer);
	Scene(SceneMode mode);
	~Scene();

	// Scene Mode
	SceneMode mode = SINGLE_PLAYER;

	// Events
	std::deque<SDL_Event> event_deque;

	// Static last  gameObject id
	static Uint16 lastGameObjectID;

	// Scene state
	bool isPaused = false;

	// Renderer
	SDL_Renderer *renderer = nullptr;

	// GameObjects' lists
	std::map<Uint16, GameObject*> gameObjectMap;
	std::vector<GameObject*> gameObjectsToInitialize;
	std::vector<GameObject*> gameObjectsToDestroy;

	// Managers
	InputManager* inputManager = nullptr;
    std::vector<ManagerBase*> managers;

    template <class T>
    Manager<T>* getManager()
    {
        for (auto manager : managers)
        {
            if (Manager<T>* man = dynamic_cast<Manager<T>*>(manager))
            {
                return man;
            }
        }

        return nullptr;
    }

    template <class T>
    T* setManager(T* manager)
    {
        managers.push_back(manager);
        return manager;
    }

    template <class T>
    void addComponentToManager(T component)
    {
            if (Manager<T>* lower_manager = getManager<T>())
            {
                lower_manager->addComponent(component);
                return;
            }
    }

    template <class T>
    void removeComponentFromManager(T component)
    {
        for (auto manager : managers)
        {
            if (Manager<T>* lower_manager = dynamic_cast<Manager<T>*>(manager))
            {
                lower_manager->removeComponent(component);
            }
        }
        
    }

	using InputHistory = std::unordered_map<Uint32, InputStatus>;

	// Rendering stuff
	static SDL_Texture* frame_to_render;

	// Network stuff
	std::ofstream online_log;
	NetworkAgent *networkAgent = nullptr;
	std::unordered_map<NetworkOwner, InputStatusPacket*> last_packets;
	std::unordered_map<NetworkOwner, InputHistory> input_histories;
	bool connectionEstablished = false;
	bool stop_sending = false;
	Uint32 calc_frame_count = 0;
	Uint32 frame_count = 0;

	// Network Methods
	void addOnlineLogLine(InputStatus status, Uint32 owner, Uint32 frame);
	void setSceneMode(SceneMode sceneMode);
	void destroyNetworkAgent();
	virtual void onDisconnect() {};
	virtual void handleConnectionEstablished() {};
	bool isOnline();
	NetworkOwner getNetworkOwnership();

	bool canCalculateFrame(Uint32 frame);
	InputStatusPacket* getOldestLastPacket();
	bool shouldStopSending();

	// Overrided Methods
	virtual void loadMedia();
	virtual void start();
	virtual void onUpdate();
	virtual void onDestroy() {};
	void destroy();
	GameObject* handleEvent(const SDL_Event& event, bool from_network = false);
	virtual void OnHandleEvent(const SDL_Event& event) {};
	virtual GameObject* createGameObjectByTemplateId(int template_id) { return nullptr; };

	// GameObjects
	void addGameObject(GameObject *gameObject);
	void removeGameObject(GameObject *gameObject);
	void initGameObject(GameObject *gameObject);
	void setGameObjectToDestroy(GameObject *gameObject);
	void destroyGameObject(GameObject *gameObject);
	GameObject* getGameObjectById(int id);
	void deactivateAllGameObjects();
	void activateAllGameObjects();

	template<typename T>
	std::vector<T> getGameObjects() 
	{
		std::vector<T> gos;

		for (auto go_p : gameObjectMap)
		{
			if (GameObject* go = go_p.second)
			{
				if (T t = dynamic_cast<T>(go))
					gos.push_back(t);
			}
		}

		return gos;
	}

	// Main
	void update();
	void handle_events(std::deque<SDL_Event>& events, bool network = false);
};

