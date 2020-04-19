#ifndef _CAMERA_H_
#define _CAMERA_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <vector>
#include <memory>

#include "player.h"

class Camera {
private:
	glm::mat4 model;
	Player* tracked_player;
	float pitch;

public:

	Camera(Player* p);
	glm::mat4 getView();
	void updateView();
	void lookUpDown(float yOffset);
};

#endif