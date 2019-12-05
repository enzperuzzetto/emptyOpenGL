#pragma once

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <GL/glew.h>

#include <iostream>

/** Report error OpenGL in stderr */
inline void _check_gl_error(const char* file, int line) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR)
		std::cout << "error: " << (int)error << " in " << file << " at l." << line << std::endl;
}

#define checkError() _check_gl_error(__FILE__,__LINE__)