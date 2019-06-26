#pragma once
#include "GUIComponent.h"

#include "TextureRenderer.h"

#include "TextLabel.h"

#include <functional>

class Button : public GUIComponent
{
public:
	// Constructors
	Button();
	Button(Texture texture);
	~Button();

	BoxCollider *collider = nullptr;
	TextureRenderer *tRenderer = nullptr;

	// GameObjects
	TextLabel* tLabel = nullptr;

	// Overrided Methods
	void onStart() override;
	bool OnHandleEvent(const SDL_Event& event) override;

	// Own methods
	const bool isWithinBoundaries(int mousePosX, int mousePosY);
	void setEnabled(bool enabled);
	virtual void onSetEnabled(bool enabled) {};
	void click();

	// Listener
	std::function<void()> onClick;

	// set onClick
	void setOnClickListener(std::function<void()> callback);

protected:

	// Attributes
	bool isEnabled = true;
};

