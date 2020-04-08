#ifndef _WINDOW_H_
#define _WINDOW_H_

#ifdef __APPLE__
#define GLFW_INCLUDE_GLCOREARB
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "Object.h"
#include "Cube.h"
#include "OBJObject.h"
#include "shader.h"


class Window
{
public:
	static glm::mat4 view;
	static glm::mat4 projection;

	// Initializes your shader program(s) and uniform variable locations
	static bool initializeProgram();
	// Initialize your OBJ objects here
	static bool initializeObjects();
	// Make sure to clean up any data you dynamically allocated here
	static void cleanUp();
	// Creates a GLFW window context
	static GLFWwindow* createWindow(int width, int height);
	// Is called whenever the window is resized
	static void resizeCallback(GLFWwindow* window, int width, int height);
	// Is called on idle.
	static void idleCallback();
	// This renders to the glfw window. Add draw calls here
	static void displayCallback(GLFWwindow*);
	// Add your key press event handling here
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

#endif
