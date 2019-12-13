#include "renderer.h"
#include "cube.h"
#include "line.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace glm;
using namespace OVR;

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

	_oculusVR = new OculusVR();

	if (!_oculusVR->initVR())
	{
		assert(("Failed to initialize vr", false));
	}

	if (!_oculusVR->initBufferVR(_winWidth, _winHeight))
	{
		assert(("Failed to initialize buffer vr", false));
	}

	_oculusVR->setProjection(0.2f, 100.f);
	_oculusVR->activateAim(true);

	for (unsigned int i = 0; i < 2; i++)
	{
		_cam[i] = new Camera(_winWidth, _winHeight);
		_cam[i]->setPerspective(_oculusVR->getProjection(i));
		_cam[i]->lookAt(vec3(0.f, 0.f, -20.f) , vec3(0.f), vec3(0.f, 1.f, 0.f));
	}

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
	for(auto& cam : _cam)
		cam->reshape(width, height);
}

void Renderer::drawScene()
{
	glViewport(0, 0, _winWidth, _winHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// draw all objects 
	ovrSessionStatus sessionStatus = _oculusVR->getSessionStatus();
	
	if (sessionStatus.IsVisible)
	{
		_oculusVR->tracking();

		for (unsigned int i = 0; i < ovrEye_Count; i++)
		{
			

			_oculusVR->onRender(i);

			mat4 eyeInSpace = _oculusVR->getEyeInSpace(i);
			const mat4 currentViewProjection = _cam[i]->projectionMatrix() * (glm::inverse(eyeInSpace) *  _cam[i]->viewMatrix());
			_oculusVR->updateControllers(glm::inverse(eyeInSpace) * _cam[i]->viewMatrix());

			for (auto& mesh : _meshes)
				mesh->render(currentViewProjection);

			
			ovrInputState inputState;
			if (_oculusVR->inputControllers(inputState))
			{
				//callback button
				//inputOculus(inputState);
			}
			
			_oculusVR->renderControllers(currentViewProjection);

			_oculusVR->onRenderFinish(i);
		}

		_oculusVR->submitFrame();
		_oculusVR->blitMirror(_winWidth, _winHeight);
	}
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
		_cam[0]->trackball->start();
		_cam[0]->trackball->update(_lastMouse);
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
		_cam[0]->trackball->end();
	}
	break;
	}
}

void Renderer::mouseMoved(int x, int y)
{
	_lastMouse.x = x;
	_lastMouse.y = y;
	if (_cam[0]->trackball->isTrack())
	{
		switch (_mouseButtonFlags)
		{
		case MOUSE_BUTTON_LEFT:
		{
			float angle;
			vec3 axis;
			_cam[0]->trackball->trackRotate(ivec2(x, y), angle, axis);
			for (auto& cam : _cam)
				cam->rotateAroundTarget(angle, normalize(axis));
			
			break;
		}
		case MOUSE_BUTTON_MIDDLE:
		{
			vec3 direction;
			_cam[0]->trackball->trackTranslate(ivec2(x, y), direction);
			for (auto& cam : _cam)
				cam->translate(direction);
			break;
		}

		case MOUSE_BUTTON_RIGHT:
		{
			float angle;
			vec3 axis;
			_cam[0]->trackball->trackRotate(ivec2(x, y), angle, axis);
			for (auto& cam : _cam)
				cam->rotate(angle, normalize(axis));
			break;
		}
		default:
			break;
		}
	}
}

void Renderer::mouseScroll(double x, double y)
{
	for (auto& cam : _cam)
		cam->zoom(-y);
}

void Renderer::keyPressed(int key, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_SPACE:
		{
			if (_oculusVR->reset())
			{
				for (unsigned int i = 0; i < 2; i++)
				{
					_cam[i]->lookAt(vec3(0.f, 0.f, -20.f), vec3(0.f), vec3(0.f, 1.f, 0.f));
				}
			}
			break;
		}
		default:
			break;
		}
	}
}

void Renderer::charPressed(int key)
{
}
