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
	std::vector<Collider*> resizableColliderVector = components;

	for (int i = 0; i < components.size(); i++)
	{
		// Remove this collider from resizable list
		Collider *c1 = components[i];
		removeColliderFromVector(c1, resizableColliderVector);

		// Don't check collisions if collider is not active
		if (!c1->isEnabled)
			continue;

		// Don't check collisions if object is not active
		if (!c1->gameObject->shouldBeLoaded())
			continue;

        // If debug enabled, draw collisions
        if (c1->debug)
            c1->drawCollisionBoundaries();

		// Iterate with the rest of them
		for (int j = 0; j < resizableColliderVector.size(); j++)
		{
			Collider* c2 = resizableColliderVector[j];

            // Check if the other collider is enabled
            if (!c2->isEnabled)
                continue;

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

std::vector<Collider*> CollisionManager::removeColliderFromVector(Collider* collider, std::vector<Collider*> &vector)
{
	return Utilities::removeFromVector(collider, vector);
}