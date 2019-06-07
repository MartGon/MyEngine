#include "Timer.h"
#include "GameObject.h"

// Timer Namespace
Uint32 TimerNs::timer_callback(Uint32 delay, void* params)
{
	if (Timer* timer = static_cast<Timer*>(params))
	{
		if (TimerHandler* handler = dynamic_cast<TimerHandler*>(timer->timer_handler))
			handler->onTimerFinish(timer->param);
	}

	return 0;
}

// Constructors

Timer::Timer(int delay, TimerHandler* timer_handler, void* param) : timer_handler{ timer_handler }, param{param}
{
	sdl_timer = SDL_AddTimer(delay, TimerNs::timer_callback, this);

	if (!sdl_timer)
		std::cout << "Error when calling SDL_AddTimer:" << SDL_GetError();
}

// TimerComponent

// Constructor
TimerComponent::TimerComponent(Uint32 ms, Uint8 flag) : Component()
{
	// Set param
	this->flag = flag;

	// Calculate due date
	delay = ms;
	Uint32 now = SDL_GetTicks();
	due_date = now + delay;
};

// Methods
Uint32 TimerComponent::getTimeRemaining()
{
	Uint32 now = SDL_GetTicks();
	return isOver ? 0 : due_date - now;
}

void TimerComponent::reset()
{
	// Reset flag
	isOver = false;

	// Calculate new due date
	Uint32 now = SDL_GetTicks();
	due_date = now + delay;
}

// Overrided Methods
void TimerComponent::update()
{
	// Return if it's over
	if (isOver)
		return;

	// Check diff
	Uint32 now = SDL_GetTicks();

	// Check for trigger
	if (now > due_date)
	{
		// Set Flag
		isOver = true;

		// Call callback
		gameObject->onTimerEnd(flag);
	}
}