#include "GameObject.h"
#include "SceneManager.h"
#include "TextureRenderer.h"
#include "RendererManager.h"
#include "Animator.h"
#include "Navigator.h"

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
	// Destroy every component
	while (!components.empty())
	{
		Component* c = components.front();
		components.erase(components.begin());
		c->destroy();
		delete c;
	}

	// Delete rotation center
	if (transform.rotationCenter)
	{
		delete transform.rotationCenter;
		transform.rotationCenter = nullptr;
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
	// Hook for gameObject updates
	onUpdate();

	// Update every component
	for (auto &component : components)
		if(component->isEnabled)
			component->update();

	//if (SceneManager::scene->isOnline() && !SceneManager::scene->shouldSendGameObjectUpdate(this))
		//return;
}

// Component Management

Component* GameObject::getComponentById(unsigned int id)
{
	if (id == -1)
		return &transform;

	if (components.empty() || components.size() <= id)
		return nullptr;

	return components[id];
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

void GameObject::setCentered(Vector2<float> pos, Vector2<float> dim)
{
	auto i_dim = getDimensions();

	// Get outer object half point
	Vector2<float> half_point = pos + dim / 2;

	// Set halfway
	auto final_pos = half_point - i_dim / 2;

	if (transform.parent)
		transform.position = final_pos - transform.parent->position;
	else
		transform.position = final_pos;
}

void GameObject::setCenteredWithinParent(Vector2<float> offset)
{
	if (transform.parent)
	{
		GameObject* go = transform.parent->gameObject;

		auto dim = go->getDimensions();
		auto pos = go->transform.position;

		setCentered(pos, dim);

		transform.position = transform.position + offset;
	}
}

void GameObject::setRelativePosition(Vector2<float> pos, Vector2<float> dim, Vector2<float> margin_percent, AlignStruct align)
{
	Vector2<float> offset = dim * margin_percent / 100.f;
	Vector2<float> final_pos;

	if (align.h_align == ALIGN_FROM_RIGHT)
	{
		auto i_dim = getDimensions();
		float pre_xpos = pos.x + dim.x - offset.x - i_dim.x;
		final_pos.x = pre_xpos;
	}
	else
	{
		final_pos.x = pos.x + offset.x;
	}

	if (align.v_align == ALIGN_FROM_BOTTOM)
	{
		auto i_dim = getDimensions();
		float pre_ypos = pos.y + dim.y - offset.y - i_dim.y;
		final_pos.y = pre_ypos;
	}
	else
	{
		final_pos.y = pos.y + offset.y;
	}

	if (transform.parent)
		transform.position = final_pos - transform.parent->position;
	else
		transform.position = final_pos;
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

// Netowrk

bool GameObject::isNetworkUpdated()
{
	Transform* parent = &transform;
	while (parent)
	{
		GameObject* go = parent->gameObject;

		if (!go->isNetworkStatic)
			return true;

		parent = go->transform.parent;
	}

	return false;
}

bool GameObject::isNetworkOwned()
{
	if (SceneManager::scene->getNetworkOwnership() == network_owner)
		return true;

	// Check if any of parents should be updated
	Transform* parent = transform.parent;
	while (parent)
	{
		GameObject* go = parent->gameObject;

		if (SceneManager::scene->getNetworkOwnership() == network_owner)
			return true;

		parent = go->transform.parent;
	}

	return false;
}

void GameObject::updateGameObjectFromComponentPacket(ComponentPacket* component_packet)
{
	unsigned int component_id = component_packet->id;
	if (Component* component = getComponentById(component_id))
	{
		// Set state
		component->isEnabled = component_packet->isEnabled;
		component->updateFromComponentPacket(component_packet);
	}
}

GameObjectUpdatePacket* GameObject::toGameObjectUpdatePacket()
{
	return new GameObjectUpdatePacket(this);
}

void GameObject::updateFromGameObjectUpdatePacket(GameObjectUpdatePacket* goup)
{
	this->isActive = goup->isActive;
	//this->updateFromClient = goup->updateFromClient;
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
	isActive = false;
	SceneManager::scene->setGameObjectToDestroy(this);
}

// Collider hooks

void GameObject::onColliderEnter(Collider *collider)
{
	//printf("%i: Collision detectada con %i\n", id, collider->gameObject->id);
}

Vector2<float> GameObject::getDimensions()
{
	Vector2<float> dim;

	auto colliders = getComponents<BoxCollider>();
	if (!colliders.empty())
	{
		auto collider = colliders.back();
		dim = collider->getDimensions();
	}
	else
	{
		// Get from texture
		std::vector<TextureRenderer*> renderers = getComponents<TextureRenderer>();
		if (!renderers.empty())
		{
			TextureRenderer* tRenderer = renderers.front();
			Texture texture = tRenderer->texture;
			dim = Vector2<float>(texture.mWidth, texture.mHeight) * transform.scale;
		}
	}

	return dim;
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
	return OnHandleEvent(event);
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

// Timer hooks

void GameObject::onTimerEnd(Uint32 flag)
{

}