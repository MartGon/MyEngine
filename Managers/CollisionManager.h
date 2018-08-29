#pragma once
#include "Collider.h"
#include "Utilities.h"
#include "Manager.h"

class CollisionManager : public Manager<Collider*>
{
public:
	CollisionManager();

	// Methods
	bool checkCollisionsBetweenColliders(Collider* c1, Collider* c2);
	void manage() override;

private:

	std::vector<Collider*> removeColliderFromVector(Collider* collider, std::vector<Collider*> &vector);
};