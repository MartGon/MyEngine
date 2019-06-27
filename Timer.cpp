#include "Timer.h"
#include "GameObject.h"

#include "SceneManager.h"

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
TimerComponent::TimerComponent(Uint32 ms, Uint32 flag) : Component()
{
	// Set param
	this->flag = flag;

	// Calculate due date
	delay = ms;
	Uint32 now = getCurrentTime();
	due_date = now + delay;
};

// Methods

Uint32 TimerComponent::getCurrentTime()
{
	Uint32 now = 0;

	if (!isFrameBased)
		now = SDL_GetTicks();
	else
	{
		Uint32 frames = SceneManager::scene->frame_count;
		now = (double)frames * (double)16.6666;
	}

	return now;
}

void TimerComponent::extend(Uint32 amount)
{
	due_date += amount;
}

Uint32 TimerComponent::getTimeRemaining()
{
	Uint32 now = getCurrentTime();
	return isOver ? 0 : due_date - now;
}

void TimerComponent::reset()
{
	// Reset flag
	isOver = false;

	// Calculate new due date
	Uint32 now = getCurrentTime();
	due_date = now + delay;
}

// Overrided Methods
void TimerComponent::update()
{
	// Return if it's over
	if (isOver)
		return;

	// Return if stopped
	if (isStopped)
		return;

	// Check diff
	Uint32 now = getCurrentTime();

	// Check for trigger
	if (now > due_date)
	{
		// Set Flag
		isOver = true;

		// Call callback
		gameObject->onTimerEnd(flag);
	}
}

// TimerObject class

TimerObject::TimerObject(Uint32 ms, Uint32 flag) 
{
	timer = setComponent(new TimerComponent(ms, flag));
}

void TimerObject::setFlag(Uint32 flag)
{
	timer->flag = flag;
}

Uint32 TimerObject::getFlag() 
{
	return timer->flag;
}

void TimerObject::onTimerEnd(Uint32 flag)
{
	callback(flag);
}