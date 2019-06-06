#pragma once
#include "Texture.h"
#include "Vector2.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "Animator.h"
#include "RotatableBoxCollider.h"
#include "Packet.h"
#include <string>
#include <vector>

enum NetworkOwner
{
	OWNER_SERVER,
	OWNER_CLIENT_1,
	OWNER_CLIENT_2,
	OWNER_CLIENT_3
};

class Component;
class GameObject : public Updateable
{
public:
	GameObject();
	GameObject(Texture texture);
	~GameObject();

	// Network Flag
	NetworkOwner network_owner = OWNER_SERVER;
	bool netCreated = false;

	/*	This gameobject is part of the scenery and 
		therefore should not be updated through the network
	*/
	bool isNetworkStatic = true;

	// Name
	std::string name = "";

	// Template id
	int template_id = -1;

	// Id
	Uint16 id = 0;

	// Active
	bool isActive = true;

	// Transform
	Transform transform;

	// Components
	std::vector<Component*> components;

	// Methods

	// Init
	void start();
	virtual void onStart();

	// setComponent
	template <typename T>
	T* setComponent(T *component)
	{
		if (Component *cComponent = dynamic_cast<Component*>(component))
		{
			// Set component id
			unsigned int id = components.empty() ? 0 : components.back()->id + 1;
			cComponent->id = id;

			// Set reference
			cComponent->gameObject = this;

			// Add to the list
			components.push_back(cComponent);

			// Init once set
			cComponent->start();

			return component;
		}

		return nullptr;
	}

	// getComponent
	template <typename T>
	T* getComponent()
	{
		if (!components.empty())
		{
			for (auto component : components)
			{
				if (T* subComponent = dynamic_cast<T*>(component))
				{
					return subComponent;
				}
			}
		}
		return nullptr;
	}

	template <typename T>
	std::vector<T*> getComponents()
	{
		// Vector
		std::vector<T*> lComponents;

		for(auto component : this->components)
		{
			if (T* subComponent = dynamic_cast<T*>(component))
			{
				lComponents.push_back(subComponent);
			}
		}

		return lComponents;
	}

	Component* getComponentById(unsigned int id);

	// State
	bool shouldBeLoaded();

	// Position
	// Take the object's center as its positions, instead of upper left corner
	void setRelativePosition(Vector2<float> pos);
	// Get absolute position
	Vector2<float> getAbsolutePosition();

    // Rotation
    Vector2<int> getAbsoluteRotationCenter();
    void setAbsoluteRotationCenter(Vector2<int> center);

	// Update
	virtual void onUpdate();
	void update() override;

	// Scale 
	void setScale(Vector2<float> scale);

	// Cleaning
	virtual void destroy();

	// Network
	bool isNetworkUpdated();
	bool isNetworkOwned();
	void updateGameObjectFromComponentPacket(ComponentPacket* component_packet);
	GameObjectUpdatePacket* toGameObjectUpdatePacket();
	void updateFromGameObjectUpdatePacket(GameObjectUpdatePacket* goup);

	// Hooks
		// Collisions
	virtual void onColliderEnter(Collider *collider);
	//virtual void onColliderEnter(RotatableBoxCollider* collider);

		// Texture Renderer
	virtual void onVanish();
	virtual void onBlinkFinish();

		// Movement
	virtual void beforeMove();
	virtual void afterMove();
		// Events
	bool handleEvent(const SDL_Event& event, bool from_network = false);
	virtual bool OnHandleEvent(const SDL_Event& event) { return false; };
		// Animation
	virtual void onAnimationFinished(Animation* anim);
	virtual void beforeAnimationFrame(Animation* anim, int frameNumber);
		// Timer
	virtual void onTimerEnd(Uint8 flag);

private:

	// Bool
	bool isInitialized = false;
};

