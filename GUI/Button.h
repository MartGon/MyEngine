#pragma once
#include "GameObject.h"

#include "TextureRenderer.h"

#include "TextLabel.h"

#include <functional>

class Button : public GameObject
{
public:
	// Constructors
	Button();
	Button(Texture texture);
	~Button();

	// Attributes
	bool isEnabled = true;
	Uint8 layer = 0;

	BoxCollider *collider = nullptr;
	TextureRenderer *tRenderer = nullptr;

	// GameObjects
	TextLabel* tLabel = nullptr;

	// Overrided Methods
	void onStart() override;
	bool OnHandleEvent(const SDL_Event& event) override;

	// Own methods
	const bool isWithinBoundaries(int mousePosX, int mousePosY);
	void click();

	// Listener
	std::function<void()> onClick;

	// set onClick
	void setOnClickListener(std::function<void()> callback);
};

