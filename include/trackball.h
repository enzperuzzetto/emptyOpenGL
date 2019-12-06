#pragma once 
#include <glm/glm.hpp>

class Trackball {
public:
	Trackball();
	Trackball(int width_screen, int height_screen);
	~Trackball();

	void start();
	bool trackRotate(const glm::vec2 pos, float &angle, glm::vec3& axis);
	bool trackTranslate(const glm::vec2 pos, glm::vec3& v);
	void update(const glm::vec2 pos) { cur_pos = last_pos = pos; }
	void end();
	bool isTrack() { return arc_ball; };

	void setResolutionScreen(int width, int height) { _screenHeight = height; _screenWidth = width; }
	void setWidthScreen(int width) { _screenWidth = width; }
	void setHeightScreen(int height) { _screenHeight = height; }

private:
	glm::vec3 computeArcballVector(float x, float y);

	glm::vec2 cur_pos, last_pos;
	int _screenWidth, _screenHeight;
	bool arc_ball = false;
};