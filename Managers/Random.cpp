#include "Random.h"

// Attributes
std::random_device Random::rd;
uint64_t Random::seed = rd();
std::mt19937 Random::dre = std::mt19937(seed);
unsigned int Random::called_counter = 0;

// Constructor

Random::Random()
{

}

// Own methods

