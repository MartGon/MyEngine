#pragma once
#include "BoxCollider.h"
#include "Vector2.h"
#include <string>
#include <array>

class RotatableBoxCollider : public Collider
{
public:

	// Constructors
	RotatableBoxCollider(Vector2<int> v0, Vector2<int> v1, Vector2<int> v2, Vector2<int> v3);
	RotatableBoxCollider(BoxCollider* collider);
	~RotatableBoxCollider();

	// Vertex to apply logic
	std::array<Vector2<float>, 4> vertex;

	// Methods
	void setRotation(Vector2<int> rotationCenter, double angle);
    Vector2<int> getColliderCenter();
    bool checkCollision(RotatableBoxCollider collider);

	// Debug
	std::string vertexValuesToStr();
	void drawCollisionBoundaries() override;

	// Upper
	void update() override;
	bool isCollidingWith(Collider* collider) override;

private:

	// Square Vertex - Read Only
	// v1 - v0 - v2				v1----v3
	// v0 - v1 - v3				|	   |
	// v0 - v2 - v3				|	   |
	// v1 - v3 - v2				v0----v2
    std::array<Vector2<float>, 4> roVertex;

	Vector2<float> rotateVertex(Vector2<int> rotationCenter, double angle, Vector2<float> vertex);
};