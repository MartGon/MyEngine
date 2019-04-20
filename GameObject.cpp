#include "GameObject.h"
#include "SceneManager.h"
#include "TextureRenderer.h"
#include "RendererManager.h"
#include "Animator.h"

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
	while (!components.empty())
	{
		Component* c = components.front();
		components.erase(components.begin());
		c->destroy();
		delete c;
	}
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

// State

bool GameObject::shouldBeLoaded() 
{
	if (!isActive)
		return false;

	// Don't load if any superior gameobject is not active
	Transform* parent = transform.parent;
	while (parent)
	{
		GameObject* go = parent->gameObject;

		if (!go->isActive)
			return false;
		
		parent = go->transform.parent;
	}

	return true;
}

// Position

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
    if (Collider *collider = getComponent<Collider>())
    {
        collider->update();
    }
}

Vector2<float> GameObject::getAbsolutePosition()
{
	Vector2<float> position = this->transform.position;
	Transform* current_transform = this->transform.parent;

	while (current_transform != nullptr)
	{
		position = position + current_transform->position;
		current_transform = current_transform->parent;
	}

	return position;
}

// Rotation

Vector2<int> GameObject::getAbsoluteRotationCenter()
{
	if (!transform.rotationCenter)
		transform.rotationCenter = new Vector2<int>(0, 0);

    Vector2<int> abs_center = (Vector2<int>)getAbsolutePosition() + (*transform.rotationCenter);

    return abs_center;
}

void GameObject::setAbsoluteRotationCenter(Vector2<int> center)
{
    if (transform.rotationCenter)
        *transform.rotationCenter = center - (Vector2<int>)getAbsolutePosition();
    else
        transform.rotationCenter = new Vector2<int>(center - (Vector2<int>)getAbsolutePosition());
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
	SceneManager::scene->destroyGameObject(this);
}

// Collider hooks

void GameObject::onColliderEnter(Collider *collider)
{
	//printf("%i: Collision detectada con %i\n", id, collider->gameObject->id);
}

// TextureRenderer

void GameObject::onVanish()
{

}

void GameObject::onBlinkFinish()
{

}

// Navigator hooks

void GameObject::beforeMove()
{

}

void GameObject::afterMove()
{

}

// Event hooks

bool GameObject::handleEvent(const SDL_Event &event)
{
	return false;
}

// Animation hooks

void GameObject::onAnimationFinished(Animation* anim)
{
	//printf("Animation %i has finished\n", anim->id);
}

void GameObject::beforeAnimationFrame(Animation* anim, int frameNumber)
{
	//printf("Animation %i frame %i has finished\n", anim->id, frameNumber);
}