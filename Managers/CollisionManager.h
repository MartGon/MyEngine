#pragma once
#include "Collider.h"
#include "Utilities.h"

class CollisionManager
{
public:
	CollisionManager();

	// Methods
	bool checkCollisionsBetweenColliders(Collider* c1, Collider* c2);
	void manage();

	// Modify list
	void addCollider(Collider* collider);
	void removeCollider(Collider* collider);

private:
	std::vector<Collider*> colliders;

	std::vector<Collider*> removeColliderFromVector(Collider* collider, std::vector<Collider*> &vector);
};