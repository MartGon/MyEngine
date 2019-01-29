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

void Navigator::update()
{
	// Hook
	gameObject->beforeMove();

    // If the previous hook deactivates
    if (!isEnabled)
        return;

    previousPos = gameObject->transform->position;
	Vector2<float> displacement = (direction * speed);
	Vector2<float> newPos = previousPos + displacement;
	gameObject->transform->position = newPos;

	// Is Affected by acceleration ?
	if (isKinematic)
	{
		direction.x += acceleration.x;

		// Y axis is inverted on SDL
		direction.y -= acceleration.y;

		// TODO - Esto altera el vector direccion, provocando que ya no este normalizado
		// Solucion - Restar aceleracion al displacement, coger el modulo = speed y normalizar luego
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
    gameObject->transform->position = previousPos;
}
