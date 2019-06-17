#include "Button.h"
#include "RendererManager.h"

// Constructors

Button::Button()
{
}

Button::Button(Texture texture) : GameObject(texture)
{
	// Set collider
	collider = setComponent(new BoxCollider(texture));
	tRenderer = getComponent<TextureRenderer>();

	// Init tLabel
	tLabel = new TextLabel();
	tLabel->transform.parent = &this->transform;
}

Button::~Button()
{
}

// Methods

bool Button::OnHandleEvent(const SDL_Event& event)
{
	// Check wether it is enabled
	if (!isEnabled)
		return false;

	// Get Mouse coordinates
	int x, y;
	SDL_GetMouseState(&x, &y);

	// Check if it affects this button
	if (!isWithinBoundaries(x, y))
		return false;

	// Handle event
	switch (event.type)
	{
		case SDL_MOUSEMOTION:
			break;
		case SDL_MOUSEBUTTONDOWN:
			return true;
		case SDL_MOUSEBUTTONUP:
			click();
			return true;
		default:
			break;
	}

	return false;
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

void Button::click()
{
	try
	{
		onClick();
	}
	catch (std::bad_function_call)
	{
		std::cout << "Button function has not been implemented \n";
	}
}