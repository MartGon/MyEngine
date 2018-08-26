#pragma once
#include "Texture.h"
#include "Vector2.h"
#include "Transform.h"
#include "BoxCollider.h"
#include "Animator.h"
#include "RotatableBoxCollider.h"
#include <string>
#include <vector>

class Component;
class GameObject : public Updateable
{
public:
	GameObject();
	GameObject(Texture texture);
	~GameObject();

	// Network Flag
	bool updateFromClient = false;

	// Name
	std::string name = "";

	// Id
	Uint16 id = 0;

	// Active
	bool isActive = true;

	// Transform
	Transform transform;

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
			cComponent->gameObject = this;
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

	// Position
	// Take the object's center as its positions, instead of upper left corner
	void setRelativePosition(Vector2<float> pos);

	// Update
	virtual void onUpdate();
	void update() override;

	// Scale 
	void setScale(Vector2<float> scale);

	// Cleaning
	virtual void destroy();

	// Hooks
		// Collisions
	virtual void onColliderEnter(Collider *collider);
	//virtual void onColliderEnter(RotatableBoxCollider* collider);

		// Movement
	virtual void beforeMove();
	virtual void afterMove();
		// Events
	virtual void handleEvent(const SDL_Event& event);
		// Animation
	virtual void onAnimationFinished(Animation* anim);
	virtual void beforeAnimationFrame(Animation* anim, int frameNumber);

private:

	// Components
	std::vector<Component*> components;

	// Bool
	bool isInitialized = false;
};

