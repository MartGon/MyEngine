#include "Collider.h"
#include "SceneManager.h"

Collider::Collider() : Component()
{
	// Adding to collider list
	SceneManager::scene->addComponentToManager(this);
}

void Collider::destroy() 
{
	SceneManager::scene->collisionManager->removeComponent(this);
}