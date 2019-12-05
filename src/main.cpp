#include <iostream>
#include <GL/glew.h>
#include <GLFW\glfw3.h>


#define WIDTH 1280
#define HEIGHT 960

//render to global variable

static void char_callback(GLFWwindow* window, unsigned int key)
{
}

static void scroll_callback(GLFWwindow* window, double x, double y)
{
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if ((key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
}

void cursorPos_callback(GLFWwindow* window, double x, double y)
{
}

void  reshape_callback(GLFWwindow* window, int width, int height)
{
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
	
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);

	while (!glfwWindowShouldClose(window)) {

		//render here

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glfwDestroyWindow(window);
	glfwTerminate();
	return EXIT_SUCCESS;
}