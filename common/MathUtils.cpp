#include "MathUtils.h"

float MathUtils::randfUniform(float min, float max)
{
	return min + static_cast <float> (rand()) / static_cast <float> (RAND_MAX / (max - min));
}

float MathUtils::randfNormal(float mu, float sig)
{
	return 0.0f;
}

mat4 MathUtils::lookAt(vec3 position, vec3 target, vec3 up)
{
	vec3 forward = normalize(target - position);
	vec3 right = normalize(cross(forward, up));
	up = cross(right, forward);
	return mat4(vec4(-right, 0), vec4(up, 0), vec4(forward, 0), vec4(position, 1));
}
