#include "Navigator.h"
#include "GameObject.h"

// Constructors

Navigator::Navigator()
{
	direction = Vector2<float>();
	speed = 1;
}

Navigator::Navigator(Vector2<float> direction, int speed) : Component()
{
	this->direction = direction;
	this->speed = speed;
}

Navigator::Navigator(Vector2<float> direction, int speed, bool isKinematic, Vector2<float> acceleration) : Navigator(direction, speed)
{
	this->isKinematic = isKinematic;
	this->acceleration = acceleration;
}

void Navigator::update()
{
	// Hook
	gameObject->beforeMove();

    // If the previous hook deactivates
    if (!isEnabled)
        return;

	// Set new position
    previousPos = gameObject->transform.position;
	Vector2<float> displacement = (direction * speed);
	Vector2<float> newPos = previousPos + displacement;
	gameObject->transform.position = newPos;

	// Is Affected by acceleration ?
	if (isKinematic)
	{
		// TODO - Esto altera el vector direccion, provocando que ya no este normalizado
		// Solucion - Restar aceleracion al displacement, coger el modulo = speed y normalizar luego
		displacement.x += acceleration.x;

		// Y axis is inverted on SDL
		displacement.y -= acceleration.y;

		// Get new speed value
		float new_speed = displacement.getModule();

		// Update new direction
		setDirection(displacement);

		updateNavStatus(new_speed, speed);

		// Set new speed value
		speed = new_speed;
	}

	// Hook
	gameObject->afterMove();
}

Vector2<float> Navigator::getDirection()
{
	return direction;
}

void Navigator::setDirection(Vector2<float> dir)
{
	dir.normalize();
	direction = dir;
}

void Navigator::goToPreviousPos()
{
    gameObject->transform.position = previousPos;
}

void Navigator::updateNavStatus(float cur_speed, float prev_speed)
{
	NavStatus old_status = status;

	if (cur_speed > prev_speed)
		status = NavStatus::SPEED_INCREASING;
	else if(cur_speed < prev_speed)
		status = NavStatus::SPEED_DECREASING;
	else
	{
		status = NavStatus::SPEED_NONE;
		return;
	}

	if (old_status == NavStatus::SPEED_NONE)
		return;

	if (old_status != status)
	{
		if (stopAtInflectionPoint)
		{
			isEnabled = false;
			status = NavStatus::SPEED_NONE;
		}
	}
}

ComponentPacket* Navigator::toComponentPacket()
{
	return new NavigatorPacket(this);
}

void Navigator::updateFromComponentPacket(ComponentPacket* component_packet) 
{
	// Not a correct type
	if (component_packet->sub_type != ComponentPacketType::COMPONENT_NAVIGATOR)
		return;

	// Update from data received
	NavigatorPacket* navigator_packet = static_cast<NavigatorPacket*>(component_packet);
	speed = navigator_packet->speed;
	setDirection(navigator_packet->direction);
	acceleration = navigator_packet->acceleration;
	isKinematic = navigator_packet->isKinematic;
	stopAtInflectionPoint = navigator_packet->stopAtInflectionPoint;
}