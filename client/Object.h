#ifndef _OBJECT_H_
#define _OBJECT_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>

class Object
{
protected:
	glm::mat4 model = glm::mat4(1);
	glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
public:
	glm::mat4 getModel() { return model; }
	glm::vec3 getColor() { return color; }
	void setColor(glm::vec3 c) { color = c; }

	virtual void draw(GLuint program) = 0;
	virtual void update() = 0;
};

#endif

