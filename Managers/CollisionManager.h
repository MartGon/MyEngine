#pragma once
#include "RotatableBoxCollider.h"
#include <vector>

class CollisionManager
{
public:
	CollisionManager();

	// Methods
	bool checkCollisionsBetweenColliders(RotatableBoxCollider* c1, RotatableBoxCollider* c2);
	void manage();
	void addCollider(RotatableBoxCollider* collider);

private:
	std::vector<RotatableBoxCollider*> colliders;

	std::vector<RotatableBoxCollider*> removeColliderFromVector(RotatableBoxCollider* collider, std::vector<RotatableBoxCollider*> &vector);
};