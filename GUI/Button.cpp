#include "Button.h"
#include "RendererManager.h"

// Constructors

Button::Button()
{
}

Button::Button(Texture texture) : GameObject(texture)
{
	collider = setComponent(new BoxCollider(texture));
}

Button::~Button()
{
}

// Methods

void Button::handleEvent(const SDL_Event& event)
{
	// Get Mouse coordinates
	int x, y;
	SDL_GetMouseState(&x, &y);

	// Check if it affects this button
	if (!isWithinBoundaries(x, y))
		return;

	// Handle event
	switch (event.type)
	{
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEBUTTONDOWN:
			break;
		case SDL_MOUSEBUTTONUP:
			onClick();
			break;
		default:
			break;
	}

}

const bool Button::isWithinBoundaries(int mousePosX, int mousePosY)
{
	// In case native res does not match window res
	Vector2<float> scaler = RendererManager::getScaler();
	bool inside = false;

	int x = mousePosX;
	int y = mousePosY;

	if (x < collider->cLeft * scaler.x)
		return false;
	else if (x > collider->cRight * scaler.x)
		return false;
	else if (y < collider->cTop * scaler.y)
		return false;
	else if (y > collider->cBottom * scaler.y)
		return false;
	else
		return true;
}

void Button::setOnClickListener(std::function<void()> callback)
{
	onClick = callback;
}

void Button::onStart()
{
	collider = getComponent<BoxCollider>();
}