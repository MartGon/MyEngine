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

private:
	// Pseudo Random generator
	static std::random_device rd;
	static std::mt19937 dre;
};