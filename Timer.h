#pragma once
// SDL
#include "SDL_timer.h"

// Own
#include "TimerHandler.h"

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