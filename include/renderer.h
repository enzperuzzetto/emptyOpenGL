#pragma once

#include "opengl.h"
#include "shader.h"

class Renderer
{
public:

	Renderer();
	~Renderer();

	// gl stuff
	void init(int width, int height);
	void loadShaders();
	void reshape(int width, int height);
	void drawScene();
	void updateScene();
	void draw();

	// events
	void mousePressed(GLFWwindow* window, int button, int action);
	void mouseMoved(int x, int y);
	void mouseScroll(double x, double y);
	void keyPressed(int key, int action, int mods);
	void charPressed(int key);


private:
	int _winWidth, _winHeight;

	Shader _shader;
};