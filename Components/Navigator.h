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

	// Network methods
	ComponentPacket* toComponentPacket() override;
	void updateFromComponentPacket(ComponentPacket* component_packet) override;

	Vector2<float> previousPos;
	Vector2<float> previousAbsPos{-1, -1};
	Vector2<float> prev_dir = 0;

private:
	enum class NavStatus 
	{
		SPEED_NONE,
		SPEED_INCREASING,
		SPEED_DECREASING
	};

	NavStatus status = NavStatus::SPEED_NONE;

	Vector2<float> direction;

	void updateNavStatus(float curr_speed, float prev_speed);
};