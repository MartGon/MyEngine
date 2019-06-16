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

		return uid(dre);
	}

	template <typename T>
	static T getRandomUniformInteger(T l, T r)
	{
		std::uniform_int_distribution<T> uid(l,r);
		
		return uid(dre);
	}

	static void setSeed(uint64_t seed)
	{
		dre = std::mt19937(seed);
		Random::seed = seed;
	}

	static bool getBoolWithChance(float chance)
	{
		int lLimit = 1;
		int rLimit = 1 / chance;

		int result = getRandomUniformInteger(lLimit, rLimit);

		return result;
	}

	static uint64_t seed;

private:
	// Pseudo Random generator
	static std::random_device rd;
	static std::mt19937 dre;
};