#pragma once
#include <random>

class Random
{
public:
	// Constructor
	Random();

	template <typename T>
	static T getRandomUniformFloat(T l, T r)
	{
		std::uniform_real_distribution<T> uid(l, r);

		called_counter++;

		return uid(dre);
	}

	template <typename T>
	static T getRandomUniformInteger(T l, T r)
	{
		std::uniform_int_distribution<T> uid(l,r);
		
		called_counter++;

		return uid(dre);
	}

	static void setSeed(uint64_t seed)
	{
		dre = std::mt19937(seed);
		Random::seed = seed;

		// Reset counter
		called_counter = 0;
	}

	static uint64_t getSeed()
	{
		return Random::seed;
	}

	static bool getBoolWithChance(float chance)
	{
		int lLimit = 1;
		int rLimit = 1 / chance;

		int result = getRandomUniformInteger(lLimit, rLimit);

		return result;
	}

	static uint64_t seed;
	static std::mt19937 dre;

	static unsigned int called_counter;

private:
	// Pseudo Random generator
	static std::random_device rd;
	
};