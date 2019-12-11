#include "renderer.h"
#include "cube.h"

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

	glClearColor(0.0, 0.0, 0.0, 0.0);

	loadShaders();

	_cam = new Camera(_winWidth, _winHeight);
	_cam->setPerspective(45.f, 0.2f, 100.f, (float)_winWidth / (float)_winHeight);
	_cam->lookAt(vec3(0.f, 0.f, -15.f), vec3(0.f), vec3(0.f, 1.f, 0.f));

	// init all object in scene
	Mesh* object1 = new Cube(10);
	object1->init();
	object1->setShader(_shader);
	_meshes.push_back(object1);

	glEnable(GL_DEPTH_TEST);
}

void Renderer::loadShaders()
{
	//load all shaders what do you want
	_shader.loadFromFiles(DATA_DIR"/Shaders/simple.vert", DATA_DIR"/Shaders/simple.frag");

	checkError();
}

void Renderer::reshape(int width, int height)
{
	//think to recompute viewport and matrix projection
	_winWidth = width;
	_winHeight = height;
	_cam->reshape(width, height);
	glViewport(0, 0, _winWidth, _winHeight);
}

void Renderer::drawScene()
{
	glViewport(0, 0, _winWidth, _winHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw all objects 
	mat4 viewProjection = _cam->projectionMatrix() * _cam->viewMatrix();
	for (auto mesh : _meshes)
		mesh->render(viewProjection);
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
		_cam->trackball->start();
		_cam->trackball->update(_lastMouse);
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
		_cam->trackball->end();
	}
	break;
	}
}

void Renderer::mouseMoved(int x, int y)
{
	_lastMouse.x = x;
	_lastMouse.y = y;
	if (_cam->trackball->isTrack())
	{
		switch (_mouseButtonFlags)
		{
		case MOUSE_BUTTON_LEFT:
		{
			float angle;
			vec3 axis;
			_cam->trackball->trackRotate(ivec2(x, y), angle, axis);
			_cam->rotateAroundTarget(angle, normalize(axis));
			break;
		}
		case MOUSE_BUTTON_MIDDLE:
		{
			vec3 direction;
			_cam->trackball->trackTranslate(ivec2(x, y), direction);
			_cam->translate(direction);
			break;
		}

		case MOUSE_BUTTON_RIGHT:
		{
			float angle;
			vec3 axis;
			_cam->trackball->trackRotate(ivec2(x, y), angle, axis);
			_cam->rotate(angle, normalize(axis));
			break;
		}
		default:
			break;
		}
	}
}

void Renderer::mouseScroll(double x, double y)
{
	_cam->zoom(-y);
}

void Renderer::keyPressed(int key, int action, int mods)
{
}

void Renderer::charPressed(int key)
{
}
