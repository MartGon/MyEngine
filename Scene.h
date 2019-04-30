#pragma once
#include "GameObject.h"
#include "RendererManager.h"
#include "CollisionManager.h"
#include "NetworkServer.h"
#include "NetworkClient.h"
#include <SDL_thread.h>
#include <SDL.h>
#include <vector>
#include <map>

int recvPacketThread(void *data);

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

	// Network stuff
	std::vector<Packet*> recv_packets;
	NetworkAgent *networkAgent = nullptr;
	bool connectionEstablished = false;
	bool disconnected = false;
	bool alreadyDestroyed = false;
	Vector2<int> pair_mouse_state;

	// Thread
	SDL_Thread *thread = nullptr;
	SDL_sem *sem = nullptr;

	// Static last  gameObject id
	static Uint16 lastGameObjectID;

	// Scene state
	bool isPaused = false;

	// Renderer
	SDL_Renderer *renderer = nullptr;

	// GameObject map
	std::map<Uint16, GameObject*> gameObjectMap;

	// GameObjects to initialize
	std::vector<GameObject*> gameObjectsToInitialize;

	// GameObjects to be destroy
	std::vector<GameObject*> gameObjectsToDestroy;

	// Debug
	uint64_t frame_count = 0;

	// Managers
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

	// Network Methods
	void setSceneMode(SceneMode sceneMode);
	void destroyNetworkAgent();
	void disconnect();
	virtual void onDisconnect() {};
	virtual void handleConnectionEstablished() {};
	bool isOnline();
	bool shouldSendGameObjectUpdate(GameObject* go);
	void sendGameObjectUpdate(GameObject* go);
	bool handlePacket(Packet *packet);

	// Overrided Methods
	virtual void loadMedia();
	virtual void start();
	virtual void beforeUpdate();
	virtual void onUpdate();
	virtual void destroy();
	void handleEvent(const SDL_Event& event);
	virtual void OnHandleEvent(const SDL_Event& event) {};
	virtual GameObject* createGameObjectByTemplateId(int template_id) { return nullptr; };

	void addGameObject(GameObject *gameObject);
	void removeGameObject(GameObject *gameObject);
	void initGameObject(GameObject *gameObject);
	void destroyGameObject(GameObject *gameObject);
	GameObject* getGameObjectById(int id);
	void update();
	void deactivateAllGameObjects();
	void activateAllGameObjects();
};

