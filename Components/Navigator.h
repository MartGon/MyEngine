#pragma once
#include "Component.h"
#include "Vector2.h"

class Navigator : public Component
{
public:

	// Constructors
	Navigator();
	Navigator(Vector2<float> direction, int speed);
	Navigator(Vector2<float> direction, int speed, bool isKinematic, Vector2<float> acceleration);
	~Navigator();

	// Attributes
	float speed = 0;

	// Kinematics
	bool isKinematic = false;
	bool stopAtInflectionPoint = false;
	Vector2<float> acceleration;

	// Methods
	void update() override;

	// Own Methods
	Vector2<float> getDirection();
	void setDirection(Vector2<float> dir);
    void goToPreviousPos();

private:
	enum class NavStatus 
	{
		SPEED_NONE,
		SPEED_INCREASING,
		SPEED_DECREASING
	};

	NavStatus status = NavStatus::SPEED_NONE;

	Vector2<float> direction;
    Vector2<float> previousPos;

	void updateNavStatus(float curr_speed, float prev_speed);
};