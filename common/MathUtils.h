#pragma once

#include <random>

class MathUtils {
private:

public:
	static float randfUniform(float min, float max);

	static float randfNormal(float mu, float sig);
};