#pragma once
#include "shader.h"
#include "camera.h"
#include "mesh.h"

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
	enum mouse_button
	{
		MOUSE_BUTTON_NONE = 0,
		MOUSE_BUTTON_LEFT = (1 << 0),
		MOUSE_BUTTON_RIGHT = (1 << 1),
		MOUSE_BUTTON_MIDDLE = (1 << 2)
	};

	int _winWidth, _winHeight;

	Shader _shader;

	Camera* _cam;

	std::vector<Mesh*> _meshes;

	glm::vec2 _lastMouse;
	int _mouseButtonFlags = 0;
};