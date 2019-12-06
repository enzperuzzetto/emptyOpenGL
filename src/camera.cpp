#include <camera.h>

#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtx\norm.hpp>

using namespace glm;

Camera::Camera() : 
	trackball(new Trackball()),
	_target(vec3(0.f,0.f,5.f)),
	_fov(45.f),
	_far(1000.f),
	_near(0.2f),
	_width(100),
	_height(100),
	_projectionMatrix(mat4(1.f)),
	_viewMatrix(mat4(1.f))
{
}

Camera::Camera(int width, int height) :
	Camera()
{
	_width = width;
	_height = height;
	trackball->setResolutionScreen(_width, _height);
}

Camera::~Camera()
{
}

void Camera::reshape(int width, int height)
{
	_width = width;
	_height = height;
	float ratio = (float)width / (float)height;
	setPerspective(_fov, _near, _far, ratio);
	trackball->setResolutionScreen(_width, _height);
}

void Camera::lookAt(vec3 pos, vec3 target, vec3 up)
{
	_target = target;
	_viewMatrix = glm::lookAt(pos, target, up);
}

void Camera::setPerspective(float fov, float Near, float Far, float aspectRatio)
{
	_projectionMatrix = perspective(fov, aspectRatio, Near, Far);
	_fov = fov;
	_near = Near;
	_far = Far;
}

const mat4 Camera::projectionMatrix() const
{
	return _projectionMatrix;
}

mat4 Camera::viewMatrix() const
{
	return _viewMatrix;
}

void Camera::zoom(float t)
{
	vec3 dir = direction();
	dir = normalize(dir);
	vec3 tmp = dir * (6371.f * 2.f *  t * 0.25f);
	_viewMatrix = glm::translate(_viewMatrix, tmp);
}

void Camera::rotateAroundTarget(float angle, vec3 axis)
{
	//mat4 rotate = glm::rotate(mat4(1.0f), angle, axis);
	_viewMatrix = glm::rotate(_viewMatrix, angle, axis); //rotate * _viewMatrix;//
	//_target = rodrigueRotation(_target, angle, axis);
}

void Camera::rotate(float angle, glm::vec3 axis)
{
	vec3 dir = direction();
	mat4 rotate = glm::rotate(mat4(1.0f), angle, axis);
	mat4 translate = glm::translate(mat4(1.0f), _target);
	_viewMatrix = translate * rotate * inverse(translate) * _viewMatrix;//glm::translate(glm::rotate(glm::translate(_viewMatrix, -dir), angle, axis), dir);
}

void Camera::translate(glm::vec3 v)
{
	mat4 translate = glm::translate(mat4(1.0f), v * 6371.f * 0.14f);
	_viewMatrix = translate * _viewMatrix;//glm::translate(_viewMatrix, v * 6371.f * 0.14f);
	//_target += v;
}


//https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula
vec3 Camera::rodrigueRotation(vec3 v, float angle, vec3 axis)
{
	vec3 vrot = cos(angle) * v + (1 - cos(angle)) * (dot(axis, v)) * axis + sin(angle) * (cross(axis, v));
	return vrot;
}
