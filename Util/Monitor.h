#pragma once
#include <SDL.h>


template <typename T>
class Monitor
{
public:
	// Constructor
	Monitor() {};
	Monitor(T value) : value{ value }
	{
		mutex = SDL_CreateMutex();
	};
	Monitor(const Monitor& Value) = delete;
	~Monitor()
	{
		SDL_LockMutex(mutex);
		SDL_DestroyMutex(mutex);
	};

	T get_value()
	{
		// Lock 
		SDL_LockMutex(mutex);

		T copy = value;

		// Unlock
		SDL_UnlockMutex(mutex);

		return copy;
	}

	void set_value(T value)
	{
		// Lock
		SDL_LockMutex(mutex);

		this->value = value;

		// Unlock
		SDL_UnlockMutex(mutex);
	}

	void lock()
	{
		SDL_LockMutex(mutex);
	}


	void unlock()
	{
		SDL_UnlockMutex(mutex);
	}

private:

	// SDL Mutex
	SDL_mutex* mutex;

	// Value
	T value;
};