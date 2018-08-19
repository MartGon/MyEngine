#pragma once
#include "Component.h"
#include "Vector2.h"

class Navigator : public Component
{
public:

	// Constructors
	Navigator();
	Navigator(Vector2<float> direction, int speed);
	~Navigator();

	// Attributes
	int speed;

	// Kinematics
	bool isKinematic = false;
	Vector2<float> acceleration;

	// Methods
	void update() override;

	// Own Methods
	Vector2<float> getDirection();
	void setDirection(Vector2<float> dir);

private:
	Vector2<float> direction;
};