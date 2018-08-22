#pragma once
#include "Component.h"
#include "Vector2.h"
#include <string>

class RotatableBoxCollider : public Component
{
public:

	// Constructors
	RotatableBoxCollider(Vector2<int> v1, Vector2<int> v2, Vector2<int> v3, Vector2<int> v4);

	// Offset from transform position
	Vector2<float> offset;

	// Square Vertex
	// v2 - v1 - v3
	Vector2<int> v1;
	// v4 - v2 - v1
	Vector2<int> v2;
	// v1 - v3 - v4
	Vector2<int> v3;
	// v3 - v4 - v2
	Vector2<int> v4;

	Vector2<int>* vertex[4];

	// Dir vectors
	// A = V1 -> V2
	Vector2<float> A;

	// B = V1 -> V3
	Vector2<float> B;

	// Methods
	void rotate(Vector2<int> rotationCenter, double angle);

	// Debug
	std::string vertexValuesToStr();
	void draw();

private:

	Vector2<int> rotateVertex(Vector2<int> rotationCenter, double angle, Vector2<int> vertex);
};