#include "CollisionManager.h"
#include "GameObject.h"
#include <algorithm>


CollisionManager::CollisionManager()
{

}

bool CollisionManager::checkCollisionsBetweenColliders(RotatableBoxCollider* c1, RotatableBoxCollider* c2)
{
	return c1->checkCollision(*c2) || c2->checkCollision(*c1);
}

void CollisionManager::manage()
{
	// Already checked Colliders will be removed from this vector
	std::vector<RotatableBoxCollider*> resizableColliderVector = colliders;

	for (int i = 0; i < colliders.size(); i++)
	{
		// Remove this collider from resizable list
		RotatableBoxCollider *c1 = colliders[i];
		removeColliderFromVector(c1, resizableColliderVector);

		// Iterate with the rest of them
		for (int j = 0; j < resizableColliderVector.size(); j++)
		{
			RotatableBoxCollider* c2 = resizableColliderVector[j];

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

void CollisionManager::addCollider(RotatableBoxCollider *collider)
{
	colliders.push_back(collider);
}

std::vector<RotatableBoxCollider*> CollisionManager::removeColliderFromVector(RotatableBoxCollider* collider, std::vector<RotatableBoxCollider*> &vector)
{
	vector.erase(std::remove(vector.begin(), vector.end(), collider), vector.end());
	return vector;
}