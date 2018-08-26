#pragma once
#include "Vector2.h"
#include "RendererManager.h"
#include "Component.h"

class Collider : public Component
{
public:
	// Constructor
	Collider();

	// Collider offset
	Vector2<float> offset;

	// Check method
	virtual bool isCollidingWith(Collider* collider) { return false; };

	// Debug
	virtual void drawCollisionBoundaries() {};

	// Upper methods
	void update() override {};
	void destroy() override;
};
