#include "main.h"
#include "network_connection.cpp"

void errorCallback(int error, const char* description)
{
	// Print error.
	std::cerr << description << std::endl;
}

void setupCallbacks(GLFWwindow* window)
{
	// Set the error callback.
	glfwSetErrorCallback(errorCallback);
	// Set the key callback.
	glfwSetKeyCallback(window, Window::keyCallback);
	// Set the window resize callback.
	glfwSetWindowSizeCallback(window, Window::resizeCallback);
}

void setupOpenglSettings()
{
	// Enable depth buffering.
	glEnable(GL_DEPTH_TEST);
	// Related to shaders and z value comparisons for the depth buffer.
	glDepthFunc(GL_LEQUAL);
	// Set polygon drawing mode to fill front and back of each polygon.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	// Set clear color to black.
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void printVersions()
{
	// Get info of GPU and supported OpenGL version.
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL version supported: " << glGetString(GL_VERSION)
		<< std::endl;

	//If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
	std::cout << "Supported GLSL version is: " <<
		glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}

int main(int argc, char** argv)
{
	// NETWORK INITIALIZATION

	// Validate the parameters
	if (argc != 2) {
		// printf("usage: %s server-name\n", argv[0]);
		std::cout << "usage: " << argv[0] << " server-name" << std::endl;
		return 1;
	}

	WSADATA wsaData;
	SOCKET ConnectSocket = INVALID_SOCKET;
	struct addrinfo* result = NULL,
		* ptr = NULL,
		hints;
	char sendbuf[DEFAULT_BUFLEN] = "I'm client";
	char recvbuf[DEFAULT_BUFLEN];
	int iResult;
	int recvbuflen = DEFAULT_BUFLEN;

	int connection_result = set_up_client_socket(argv[1], &wsaData, &ConnectSocket, result, ptr, &hints, &iResult);
	if (connection_result == 1)
	{
		return 1;
	}

	if (ConnectSocket == INVALID_SOCKET) {
		std::cout << "invalid socket" << std::endl;
	}

	// Send an initial buffer
	iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
	if (iResult == SOCKET_ERROR) {
		// printf("send failed with error: %d\n", WSAGetLastError());
		std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
		closesocket(ConnectSocket);
		WSACleanup();
		return 1;
	}


	// -------- GRAPHICS INITIALIZATION

	// printf("Bytes Sent: %ld\n", iResult);
	std::cout << "Bytes Sent " << iResult << std::endl;

	// Create the GLFW window.
	GLFWwindow* window = Window::createWindow(640, 480);
	if (!window) exit(EXIT_FAILURE);

	// Print OpenGL and GLSL versions.
	printVersions();
	// Setup callbacks.
	setupCallbacks(window);
	// Setup OpenGL settings.
	setupOpenglSettings();
	// Initialize the shader program; exit if initialization fails.
	if (!Window::initializeProgram()) exit(EXIT_FAILURE);
	// Initialize objects/pointers for rendering; exit if initialization fails.
	if (!Window::initializeObjects()) exit(EXIT_FAILURE);

	// Loop while GLFW window should stay open.
	while (!glfwWindowShouldClose(window))
	{
		std::string temp = "";

		if (((Window::keys[GLFW_KEY_W]) & 0xF) == GLFW_PRESS) {
			temp += "1";
		}
		else {
			temp += "0";
		}
		if (((Window::keys[GLFW_KEY_A]) & 0xF) == GLFW_PRESS) {
			temp += "1";
		}
		else {
			temp += "0";
		}
		if (((Window::keys[GLFW_KEY_S]) & 0xF) == GLFW_PRESS) {
			temp += "1";
		}
		else {
			temp += "0";
		}
		if (((Window::keys[GLFW_KEY_D]) & 0xF) == GLFW_PRESS) {
			temp += "1";
		}
		else {
			temp += "0";
		}

		sprintf(sendbuf, "%s", temp.c_str());

		// Read user input ----------------------------------------------------
		/*std::cout << "Enter message to sent: " << std::endl;
		std::cin >> sendbuf;
		if (strcmp(sendbuf, "quit") == 0) break;*/
		// End of read user input ---------------------------------------------

		// Send user input to server ------------------------------------------
		iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
		if (iResult == SOCKET_ERROR) {
			// printf("send failed with error: %d\n", WSAGetLastError());
			std::cout << "send failed with error: " << WSAGetLastError() << std::endl;
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		// End of send user input to server -----------------------------------

		// Receive result from server -----------------------------------------
		iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0) {
			// printf("Bytes received: %d\n", iResult);
			// printf("Message recived: %s\n", recvbuf);
			std::cout << "Bytes received: " << iResult << std::endl;
			std::cout << "Message received: " << recvbuf << std::endl;

			float* f_buf = (float *)(recvbuf);
			float x = f_buf[0];
			float y = f_buf[1];
			Window::currPos = glm::vec3(x, y, 0);
		}
		else if (iResult == 0)
		{
			// printf("Connection closed\n");
			std::cout << "Connection closed" << std::endl;
		}
		else
		{
			// printf("recv failed with error: %d\n", WSAGetLastError());
			std::cout << "recv failed with error: " << WSAGetLastError() << std::endl;
		}
		// End of receive result from the server ------------------------------

		// Start updating -----------------------------------------------------

		// Idle callback. Updating objects, etc. can be done here.
		Window::idleCallback();

		// Main render display callback. Rendering of objects is done here.
		Window::displayCallback(window);
	}
	
	// shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        std::cout << "shutdown failed with error: " << WSAGetLastError() << std::endl;
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }

    // Receive until the peer closes the connection
    do {

        iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
        if ( iResult > 0 )
            printf("Bytes received: %d\n", iResult);
        else if ( iResult == 0 )
            printf("Connection closed\n");
        else
            printf("recv failed with error: %d\n", WSAGetLastError());

    } while( iResult > 0 );


    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();


	Window::cleanUp();
	// Destroy the window.
	glfwDestroyWindow(window);
	// Terminate GLFW.
	glfwTerminate();

	exit(EXIT_SUCCESS);
}
