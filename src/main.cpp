#include <iostream>

#include "renderer.h"


#define WIDTH 1280
#define HEIGHT 960

//render to global variable
std::unique_ptr<Renderer> pRender;

static void char_callback(GLFWwindow* window, unsigned int key)
{
	pRender->charPressed(key);
}

static void scroll_callback(GLFWwindow* window, double x, double y)
{
	pRender->mouseScroll(x, y);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}

	pRender->keyPressed(key, action, mods);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	pRender->mousePressed(window, button, action);
}

void cursorPos_callback(GLFWwindow* window, double x, double y)
{
	pRender->mouseMoved(x, y);
}

void  reshape_callback(GLFWwindow* window, int width, int height)
{
	pRender->reshape(width, height);
}

void error_callback(int error, const char* message) {

	fputs(message, stderr);
}

GLFWwindow* init_GLFW() {
	glfwSetErrorCallback(error_callback);

	if (!glfwInit()) {
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, (GLint)GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);

	GLFWwindow* win = glfwCreateWindow(WIDTH, HEIGHT, "empty GL", NULL, NULL);
	if (!win) {

		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwMakeContextCurrent(win);
	std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;


	glewExperimental = GL_TRUE;
	GLenum err = glewInit();

	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));

		while (true);
		exit(EXIT_FAILURE);
	}

	glfwSetKeyCallback(win, key_callback);
	glfwSetCharCallback(win, char_callback);
	glfwSetFramebufferSizeCallback(win, reshape_callback);
	glfwSetCursorPosCallback(win, cursorPos_callback);
	glfwSetMouseButtonCallback(win, mouse_button_callback);
	glfwSetScrollCallback(win, scroll_callback);

	glfwSwapInterval(0);

	return win;
}

int main(int argc, char* argv[]) {

	GLFWwindow* window = init_GLFW();

	pRender = std::make_unique<Renderer>();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	pRender->init(width, height);

	while (!glfwWindowShouldClose(window)) {

		//render here
		pRender->draw();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}