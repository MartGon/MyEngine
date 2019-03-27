#include "Timer.h"

// Timer Namespace
Uint32 TimerNs::timer_callback(Uint32 delay, void* params)
{
	if (Timer* timer = static_cast<Timer*>(params))
	{
		if (TimerHandler* handler = timer->timer_handler)
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