#pragma once
#include "Component.h"
#include "Vector2.h"

// Class to handle position, scale and rotation
class Transform : public Component
{
public:
	Transform();

	Transform* parent = nullptr;
	Vector2<float> position;
	Vector2<float> scale;
	double zRotation;
	Vector2<int>* rotationCenter;
	
	void update() override {};

	// Network methods
	ComponentPacket* toComponentPacket() override;
	void updateFromComponentPacket(ComponentPacket* component_packet) override;
};