#ifndef _POINT_CLOUD_H_
#define _POINT_CLOUD_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <vector>
#include <string>
#include <iostream>

#include "Object.h"
#include "stb_image.h"

class OBJObject : public Object
{
private:
	std::vector<glm::vec3> points;
	std::vector<glm::vec3> normals;
	std::vector<glm::vec2> uvs;
	std::vector<GLuint> indices;
	GLuint vao, vbo_v, vbo_n, vbo_t, ebo, texture;
	GLfloat pointSize;
public:
	OBJObject(std::string objFilename);
	~OBJObject();

	void loadTexure(std::string filename);

	void draw(GLuint program);
	void update();

	void updatePointSize(GLfloat size);
	void spin(float deg);

	void parse(const char* filename);
	void setTranslate(glm::vec3 position);
	void setScaleRot(glm::vec3 scale, float deg, glm::vec3 axis);

	void setPositionDirection(glm::vec3 position, glm::vec3 direction, glm::vec3 up);

	void setPositionDirection(glm::vec3 position, glm::vec3 direction);

	std::vector<glm::vec3> getPoints() { return points; }
	std::vector<glm::vec3> getNormals() { return normals; }
	GLfloat getPointSize() { return pointSize; }
};

#endif

                                                                                                                                                                                                                                                                                                                                                                                      