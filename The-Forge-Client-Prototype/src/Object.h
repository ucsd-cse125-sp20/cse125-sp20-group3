#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>

#include "../The-Forge/Common_3/OS/Math/MathTypes.h"

class Object
{
public:
	mat4 model = mat4::identity();

	virtual void update(float deltaTime) = 0;
	virtual void draw(Cmd* commands) = 0;
};

#endif

