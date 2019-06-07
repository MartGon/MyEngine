#pragma once
// SDL
#include "SDL_timer.h"

// Own
#include "TimerHandler.h"
#include "Component.h"

// Standard C++
#include <iostream>

namespace TimerNs
{
	Uint32 timer_callback(Uint32 delay, void* param);
}

class Timer
{
public:
	// Constructors
	Timer(int delay, TimerHandler* timer_handler, void* param);

	// Attributes
	SDL_TimerID sdl_timer;
	TimerHandler* timer_handler;
	void* param;
};

class TimerComponent : public Component
{
public:
	// Constructor
	TimerComponent(Uint32 ms, Uint8 flag = 0) ;

	// Attributes
	Uint32 delay = 0;
	Uint8 flag = 0;
	Uint32 due_date = 0;
	bool isOver = false;

	// Methods
	void extend(Uint32 amount);
	Uint32 getTimeRemaining();
	void reset();

	// Overrided methods
	void update() override;
};