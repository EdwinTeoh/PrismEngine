#pragma once

#include <random>

/*class Random
{
	static std::mt19937 _Engine;
	static std::uniform_int_distribution<std::mt19937::result_type> _Distribution;

public:
	static void Init()
	{
		_Engine.seed(std::random_device()());
	}

	static float Float()
	{
		// Should return a number between 0 and 1
		return (float)_Distribution(_Engine) / (float)std::numeric_limits<uint32_t>::max();
	}
};*/

class Random
{
	static std::default_random_engine _Engine;
	static std::uniform_real_distribution<float> _Distribution;

public:
	// Returns a float between 0 and 1
	static float Float()
	{
		return _Distribution(_Engine);
	}

	// Returns a float between -1 and 1
	static float FloatRanged()
	{
		std::uniform_real_distribution<float> dis(-1.0, 1.0);
		return dis(_Engine);
	}
};