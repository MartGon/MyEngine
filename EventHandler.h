#pragma once
#include "Component.h"
#include <SDL.h>

class EventHandler
{
public: 
	// Constructors
	EventHandler();

	// Method
	virtual void handleEvent(const SDL_Event& event) = 0;
};