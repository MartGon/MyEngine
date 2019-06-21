#pragma once
// SDL
#include "SDL_timer.h"

// Own
#include "TimerHandler.h"
#include "Component.h"
#include "GameObject.h"

// Standard C++
#include <iostream>
#include <functional>


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
	TimerComponent(Uint32 ms, Uint32 flag = 0);

	// Attributes
	Uint32 delay = 0;
	Uint32 flag = 0;
	Uint32 due_date = 0;
	bool isOver = false;

	// Caculation method
	bool isFrameBased = true;

	// Methods
	Uint32 getCurrentTime();
	void extend(Uint32 amount);
	Uint32 getTimeRemaining();
	void reset();

	// Overrided methods
	void update() override;
	void destroy() override {};
};

class TimerObject : public GameObject
{
public:
	// Constructor
	TimerObject(Uint32 ms, Uint32 flag = 0);

	// Member
	TimerComponent* timer = nullptr;

	// Flag getters and setters
	void setFlag(Uint32 flag);
	Uint32 getFlag();

	// Callback method
	std::function<void(Uint32)> callback;

	// Overrided methods
	void onTimerEnd(Uint32 flag) override;
};