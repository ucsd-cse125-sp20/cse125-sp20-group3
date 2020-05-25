#pragma once

#include "../client/The-Forge/Common_3/OS/Math/MathTypes.h"
#include <random>

class MathUtils {
private:

public:
	static float randfUniform(float min, float max);

	static float randfNormal(float mu, float sig);

	static mat4 lookAt(vec3 position, vec3 target, vec3 up);
};