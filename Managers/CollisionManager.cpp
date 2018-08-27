#include "CollisionManager.h"
#include "GameObject.h"
#include <algorithm>


CollisionManager::CollisionManager()
{

}

bool CollisionManager::checkCollisionsBetweenColliders(Collider* c1, Collider* c2)
{
	return c1->isCollidingWith(c2) || c2->isCollidingWith(c1);
}

void CollisionManager::manage()
{
	// Already checked Colliders will be removed from this vector
	std::vector<Collider*> resizableColliderVector = colliders;

	for (int i = 0; i < colliders.size(); i++)
	{
		// Remove this collider from resizable list
		Collider *c1 = colliders[i];
		removeColliderFromVector(c1, resizableColliderVector);

		// If debug enabled, draw collisions
		if (c1->debug)
			c1->drawCollisionBoundaries();

		// Don't check collisions if object is not active
		if (!c1->gameObject->isActive)
			continue;

		// Iterate with the rest of them
		for (int j = 0; j < resizableColliderVector.size(); j++)
		{
			Collider* c2 = resizableColliderVector[j];

			// Check if collision exists
			if (checkCollisionsBetweenColliders(c1, c2))
			{
				// Call hooks
				c1->gameObject->onColliderEnter(c2);
				c2->gameObject->onColliderEnter(c1);
			}
		}
	}
}

void CollisionManager::addCollider(Collider *collider)
{
	colliders.push_back(collider);
}

void CollisionManager::removeCollider(Collider *collider)
{
	removeColliderFromVector(collider, colliders);
}

std::vector<Collider*> CollisionManager::removeColliderFromVector(Collider* collider, std::vector<Collider*> &vector)
{
	return Utilities::removeFromVector(collider, vector);
}