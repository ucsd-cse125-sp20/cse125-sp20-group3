#include "Window.h"

int width, height;
std::string windowTitle("GLFW Starter Project");

int randos = 50;

OBJObject* player;
OBJObject* ground;
std::vector<OBJObject*> others;

glm::vec3 Window::currPos = glm::vec3(0);
glm::vec3 currVelocity = glm::vec3(0);
float acceleration = 1;
float drag = 0.1;
float lastTime = glfwGetTime();

glm::vec3 cameraOffset = glm::vec3(0, -5, 10);

glm::vec3 eye = Window::currPos + cameraOffset; // Camera position.
glm::vec3 center = Window::currPos; // The point we are looking at.
glm::vec3 up(0, 1, 0); // The up direction of the camera.
float fovy = 60;
float nearPlane = 1;
float farPlane = 1000;
glm::mat4 Window::view = glm::lookAt(eye, center, up); // View matrix, defined by eye, center and up.
glm::mat4 Window::projection; // Projection matrix.

GLuint program; // The shader program


char Window::keys[GLFW_KEY_LAST];


bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	program = LoadShaders("shaders/shader.vert", "shaders/shader.frag");
	
	// Check the shader programs.
	if (!program)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	// Activate the shader program.
	glUseProgram(program);

	return true;
}

bool Window::initializeObjects() {
	// initialize player
	player = new OBJObject("Assets/SpinningBox/SpinningBox.obj");
	player->setColor(glm::vec3(0.5, 0, 1));
	player->loadTexure("Assets/SpinningBox/SpinningBox_tex.jpg");

	// randomly add in some other boxes
	float randoRange = 50;
	for (int i = 0; i < randos; i++) {
		float x = -randoRange + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * randoRange)));
		float y = -randoRange + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (2 * randoRange)));
		float r = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
		float g = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
		float b = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
		float rot = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (glm::pi<float>())));
		float s = 0.5f + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / 1.5f));

		glm::vec3 position = glm::vec3(x, y, 0);
		if (glm::length(position) > 1.5f) {
			OBJObject* other = new OBJObject("Assets/SpinningBox/SpinningBox.obj");
			other->setTranslate(position);
			other->setScaleRot(glm::vec3(s), rot, glm::vec3(0, 0, 1));
			other->setColor(glm::vec3(r, g, b));
			other->loadTexure("Assets/SpinningBox/SpinningBox_tex.jpg");
			others.push_back(other);
		}
	}
	printf("%d\n", others.size());

	// Initialize ground
	ground = new OBJObject("Assets/Ground/Ground.obj");
	ground->loadTexure("Assets/Ground/Ground_tex.jpg");

	return true;
}

void Window::cleanUp() {
	// Deallcoate the objects.
	delete player;

	// Delete the shader programs.
	glDeleteProgram(program);
}

GLFWwindow* Window::createWindow(int width, int height) {
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window.
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int w, int h) {
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height);
#endif
	width = w;
	height = h;

	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	projection = glm::perspective(glm::radians(fovy),
		(float)width / (float)height, nearPlane, farPlane);
}

void Window::idleCallback() {
	/*
	float currTime = glfwGetTime();
	float deltaTime = currTime - lastTime;
	lastTime = currTime;

	currVelocity -= drag * currVelocity;
	if (((keys[GLFW_KEY_W]) & 0xF) == GLFW_PRESS) {
		currVelocity.y += acceleration * deltaTime;
	}
	if (((keys[GLFW_KEY_A]) & 0xF) == GLFW_PRESS) {
		currVelocity.x -= acceleration * deltaTime;
	}
	if (((keys[GLFW_KEY_S]) & 0xF) == GLFW_PRESS) {
		currVelocity.y -= acceleration * deltaTime;
	}
	if (((keys[GLFW_KEY_D]) & 0xF) == GLFW_PRESS) {
		currVelocity.x += acceleration * deltaTime;
	}
	//printf("%f %f %f\n", currVelocity.x, currVelocity.y, currVelocity.z);
	*/
	//if (glm::length(currVelocity) > 0.01) {
		//currPos += currVelocity;
		player->setPositionDirection(Window::currPos, glm::vec3(1, 0, 0));

		eye = currPos + cameraOffset;
		center = currPos;
		Window::view = glm::lookAt(eye, center, up);
	//}
}

void Window::displayCallback(GLFWwindow* window) {
	// Clear the color and depth buffers.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render the object.
	player->draw(program);
	ground->draw(program);
	for (auto other : others) {
		other->draw(program);
	}

	// Gets events, including input such as keyboard and mouse or window resizing.
	glfwPollEvents();
	// Swap buffers.
	glfwSwapBuffers(window);
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	keys[key] = (mods << 4) || (action);

	if (action == GLFW_PRESS && key == GLFW_KEY_ESCAPE) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
}