#include "Window.h"

Window::Window() {

	width = 800;
	height = 600;

	xChange = 0.0f;
	yChange = 0.0f;

	// Initialization of the keys
	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}

}

Window::Window(GLint windowWidth, GLint windowHeight) {

	width = windowWidth;
	height = windowHeight;

	xChange = 0.0f;
	yChange = 0.0f;

	// Initialization of the keys
	for (size_t i = 0; i < 1024; i++) {
		keys[i] = 0;
	}

}

int Window::Initialise() {

	// Initialise GLFW
	if (!glfwInit()) {
		printf("GLFW initialization error. \n\n");
		glfwTerminate();

		return 1;
	}

	// Setup GLFW window properties
	// OpenGL version
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

	// Core profile = No backwards compatible
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Allow forward compatibility
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	mainWindow = glfwCreateWindow(width, height, "SAMPLE TEXT", NULL, NULL);

	if (!mainWindow) {
		printf("GLFW Window creation error. \n\n");
		glfwTerminate();

		return 1;
	}

	// Get buffer size information
	glfwGetFramebufferSize(mainWindow, &bufferWidth, &bufferHeight);

	// Set context for GLEW to use
	glfwMakeContextCurrent(mainWindow);

	// Handling user Key and Mouse inputs
	createCallbacks();

	// Locks cursor and disables it
	glfwSetInputMode(mainWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	// Allow modern extention features
	glewExperimental = GL_TRUE;

	// Initialization of GLEW
	if (glewInit() != GLEW_OK) {
		printf("GLEW initialization error. \n\n");
		glfwDestroyWindow(mainWindow);
		glfwTerminate();

		return 1;
	}

	glEnable(GL_DEPTH_TEST);

	// Setup viewport size
	glViewport(0, 0, bufferWidth, bufferHeight);

	// Binds main window to it's class
	glfwSetWindowUserPointer(mainWindow, this);

	

}

void Window::createCallbacks() {

	glfwSetKeyCallback(mainWindow, handleKeys);
	glfwSetCursorPosCallback(mainWindow, handleMouse);

}

GLfloat Window::getXChange() {

	GLfloat currChange;
	if (xChange >= 10000.0f || xChange <= -10000.0f) currChange = 0.0f;
	else currChange = xChange;

	xChange = 0.0f;
	return currChange;

}

GLfloat Window::getYChange() {

	GLfloat currChange;
	if (yChange >= 10000.0f || yChange <= -10000.0f) currChange = 0.0f;
	else currChange = yChange;

	yChange = 0.0f;
	return currChange;


}

void Window::handleKeys(GLFWwindow* window, int key, int code, int action, int mode) {

	// Get's the window binded
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	// If user has pressed the ESC key
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}

	if (key >= 0 && key < 1024) {

		if (action == GLFW_PRESS) {
			theWindow->keys[key] = true;
			//printf("Pressed: %d\n", key);
		}
		else if(action == GLFW_RELEASE){
			theWindow->keys[key] = false;
			//printf("Releases: %d\n", key);

		}

	}

}

void Window::handleMouse(GLFWwindow* window, double xPos, double yPos) {

	// Get's the window binded
	Window* theWindow = static_cast<Window*>(glfwGetWindowUserPointer(window));

	// Initialise the position in the start
	if (theWindow->mouseFirstMoved) {

		theWindow->lastX = xPos;
		theWindow->lastY = yPos;
		
		theWindow->mouseFirstMoved = false;
		
	}

	// Refreshing X and Y positions
	theWindow->xChange = xPos - theWindow->lastX;
	theWindow->xChange = std::fmod(theWindow->xChange, 360);
	

	theWindow->yChange = theWindow->lastY - yPos;
	theWindow->yChange = std::fmod(theWindow->yChange, 360);


	theWindow->lastX = xPos;
	theWindow->lastY = yPos;

	printf("x: %.6f, y: %.6f \n", theWindow->xChange, theWindow->yChange);
}

Window::~Window() {

	glfwDestroyWindow(mainWindow);
	glfwTerminate();

}