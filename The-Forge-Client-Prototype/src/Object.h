#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>

#include "../The-Forge/Common_3/OS/Math/MathTypes.h"

class Object
{
protected:
	mat4 model = mat4::identity();
	vec3 color = vec3(1.0f, 1.0f, 1.0f);
public:
	mat4 getModel() { return model; }
	vec3 getColor() { return color; }
	void setColor(vec3 c) { color = c; }

	virtual void draw(Cmd* commands) = 0;
	virtual void update(float deltaTime) = 0;
};

#endif

