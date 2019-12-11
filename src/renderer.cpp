#include "renderer.h"

#include <glm/gtc/type_ptr.hpp>

using namespace glm;

Renderer::Renderer() :
	_winWidth(0),
	_winHeight(0)
{
}

Renderer::~Renderer()
{
}

void Renderer::init(int width, int height)
{
	_winWidth = width;
	_winHeight = height;

	glClearColor(0.4, 0.4, 0.4, 0.0);

	loadShaders();

	// init all object in scene
	
	glEnable(GL_DEPTH_TEST);
}

void Renderer::loadShaders()
{
	//load all shaders what do you want
	_shader.loadFromFiles(DATA_DIR"/shaders/simple.vert", DATA_DIR"/shaders/simple.frag");

	checkError();
}

void Renderer::reshape(int width, int height)
{
	//think to recompute viewport and matrix projection
	_winWidth = width;
	_winHeight = height;
}

void Renderer::drawScene()
{
	glViewport(0, 0, _winWidth, _winHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw all objects 
	_shader.activate();
	
	_shader.deactivate();
}

void Renderer::updateScene()
{
	// update all objects
}

void Renderer::draw()
{
	updateScene();
	drawScene();
}


//////////////////////////////////////////////////////////////////////////////////
////////////				EVENTS						//////////////////////////

void Renderer::mousePressed(GLFWwindow* window, int button, int action)
{
	switch (action)
	{
	case GLFW_PRESS:
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
		{
			_mouseButtonFlags |= MOUSE_BUTTON_LEFT;
		}
		break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
		{
			_mouseButtonFlags |= MOUSE_BUTTON_MIDDLE;
		}
		break;
		case GLFW_MOUSE_BUTTON_RIGHT:
		{
			_mouseButtonFlags |= MOUSE_BUTTON_RIGHT;
		}
		break;
		}
	}
	break;
	case GLFW_RELEASE:
	{
		switch (button)
		{
		case GLFW_MOUSE_BUTTON_LEFT:
		{
			_mouseButtonFlags &= ~MOUSE_BUTTON_LEFT;
		}
		break;
		case GLFW_MOUSE_BUTTON_MIDDLE:
		{
			_mouseButtonFlags &= ~MOUSE_BUTTON_MIDDLE;
		}
		break;
		case GLFW_MOUSE_BUTTON_RIGHT:
		{
			_mouseButtonFlags &= ~MOUSE_BUTTON_RIGHT;
		}
		break;
		}
	}
	break;
	}
}

void Renderer::mouseMoved(int x, int y)
{
	switch (_mouseButtonFlags)
	{
	case MOUSE_BUTTON_LEFT:
	{
			
		break;
	}
	case MOUSE_BUTTON_MIDDLE:
	{
			
		break;
	}

	case MOUSE_BUTTON_RIGHT:
	{
			
		break;
	}
	default:
		break;
	}
}

void Renderer::mouseScroll(double x, double y)
{
}

void Renderer::keyPressed(int key, int action, int mods)
{
}

void Renderer::charPressed(int key)
{
}
