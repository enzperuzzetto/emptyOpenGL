#include "renderer.h"

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
}

void Renderer::mouseMoved(int x, int y)
{
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
