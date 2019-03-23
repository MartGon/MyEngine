#include "Random.h"

// Attributes
std::random_device Random::rd;
std::mt19937 Random::dre = std::mt19937(rd());

// Constructor

Random::Random()
{

}

// Own methods

