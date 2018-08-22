#pragma once
#include "Component.h"
#include "Vector2.h"
#include <string>

class RotatableBoxCollider : public Component
{
public:

	// Constructors
	RotatableBoxCollider(Vector2<int> v0, Vector2<int> v1, Vector2<int> v2, Vector2<int> v3);

	// Offset from transform position
	Vector2<float> offset;

	// Vertex to apply logic
	Vector2<float> vertex[4];

	// Dir vectors
	// A = V1 -> V2
	Vector2<float> A;

	// B = V1 -> V3
	Vector2<float> B;

	// Methods
	void setRotation(Vector2<int> rotationCenter, double angle);

	// Debug
	std::string vertexValuesToStr();
	void draw();

private:

	// Square Vertex - Read Only
	// v1 - v0 - v2
	// v3 - v1 - v0
	// v0 - v2 - v3
	// v2 - v3 - v1
	Vector2<float> roVertex[4];	

	Vector2<float> rotateVertex(Vector2<int> rotationCenter, double angle, Vector2<float> vertex);
};