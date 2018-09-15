#pragma once
#include "Component.h"
#include "Vector2.h"
#include "Texture.h"
#include "Collider.h"
#include <vector>

class BoxCollider : public Collider
{
public:
	BoxCollider();
	BoxCollider(int width, int height);
	BoxCollider(Texture texture);

	// Collider boundaries
	int cTop = 0;
	int cBottom = 0;
	int cRight = 0;
	int cLeft = 0;

	// Collider dimensions
	int cWidth = 0;
	int cHeight = 0;

	// Methods
	void calculateColliderBoundaries();
	bool checkCollision(BoxCollider* collider);
	Vector2<float> getCollisionCenter();
	Vector2<float> getDimensions();
	Vector2<float> getOffsetVector();

	// Debug
	void drawCollisionBoundaries() override;

	// Overrided
	bool isCollidingWith(Collider* collider) override;
    void start() override;
	void update() override;
	void destroy() override;
};