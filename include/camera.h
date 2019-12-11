#pragma once

#include "trackball.h"

class Camera 
{
public:
	Camera();
	Camera(int width, int height);
	~Camera();

	void reshape(int width, int height);
	void lookAt(glm::vec3 pos, glm::vec3 target, glm::vec3 up);
	void setPerspective(float fov, float Near, float Far, float aspectRatio);

	glm::mat4 projectionMatrix();
	glm::mat4 viewMatrix();

	void zoom(float t);
	void rotateAroundTarget(float angle, glm::vec3 axis);
	void rotate(float angle, glm::vec3 axis);
	void translate(glm::vec3 v);

	// Getter & Setter
	glm::vec3 pos() { return _viewMatrix[3]; }
	glm::vec3 target() { return _target; }
	glm::vec3 direction() { return _target - pos(); }
	float Fov() { return _fov; }
	float Far() { return _far; }
	float Near() { return _near; }
	int height() { return _height; }
	int width() { return _width; }


	Trackball* trackball;
	   
private:
	glm::vec3 rodrigueRotation(glm::vec3 v, float angle, glm::vec3 axis);

	glm::mat4 _projectionMatrix;
	glm::mat4 _viewMatrix;

	glm::vec3 _target;

	float _fov, _far, _near;
	int _width, _height;
};