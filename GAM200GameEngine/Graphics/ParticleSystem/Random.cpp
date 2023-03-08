#include "Random.h"

//std::mt19937 Random::_Engine;
//std::uniform_int_distribution<std::mt19937::result_type> Random::_Distribution;

std::default_random_engine Random::_Engine;
std::uniform_real_distribution<float> Random::_Distribution;