#include "GameObject.h"
#include "SceneManager.h"
#include "TextureRenderer.h"
#include "RendererManager.h"

// Constructors

GameObject::GameObject()
{
	// Add to the scene updater
	SceneManager::scene->addGameObject(this);

	// Add reference to self in transform component
	transform = Transform();
	transform.gameObject = this;

	// Set GameObject's id
	id = Scene::lastGameObjectID++;
}

GameObject::~GameObject()
{
}

GameObject::GameObject(Texture texture) : GameObject()
{
	setComponent(new TextureRenderer(texture));
	name = texture.path;
}

// Behaviour

void GameObject::start()
{
	onStart();

	isInitialized = true;
}

void GameObject::update()
{
	// Update every component
	for (auto &component : components)
		if(component->isEnabled)
			component->update();

	// Hook for gameObject updates
	onUpdate();
}

// Setters

void GameObject::setRelativePosition(Vector2<float> pos)
{
	if (TextureRenderer *tRenderer = getComponent<TextureRenderer>())
	{	
		float xOffset = tRenderer->texture.mWidth / 2 * transform.scale.x;
		float yOffset = tRenderer->texture.mHeight / 2 * transform.scale.y;
		
		// Check if position will be valid
		if (pos.x - xOffset >= 0)
			pos.x -= xOffset;
		else
			pos.x = 0;

		if (pos.y - yOffset >= 0)
			pos.y -= yOffset;
		else
			pos.y = 0;

		transform.position = pos;
	}
}

void GameObject::setScale(Vector2<float> scale)
{
	transform.scale = scale;
}

// Behaviour hooks

void GameObject::onStart()
{

}

void GameObject::onUpdate()
{

}

void GameObject::destroy()
{
	//texture.free();
	for (auto &component : components)
		component->destroy();

	this->~GameObject();
}

// Collider hooks

void GameObject::onColliderEnter(Collider *collider)
{

}

// Navigator hooks

void GameObject::beforeMove()
{

}

void GameObject::afterMove()
{

}