#pragma once

#include <limits>
#include <glm\glm.hpp>

class Mesh;

class Ray
{
public:
	Ray(glm::vec3 pos = glm::vec3(0.f), glm::vec3 dir = glm::vec3(0.f, 0.f, 1.f)) :
		_origin(pos),
		_direction(dir)
	{};
	~Ray() {};

	glm::vec3 at(float t) const { return _origin + t * _direction; }
	bool foundIntersection() const { return _t < std::numeric_limits<float>::max(); }

	void setT(float t) { _t = t; }
	glm::vec3 positionOfIntersection() { return at(_t); }
	Mesh* shape() { return _shape; }
	void setShape(Mesh* shape) { _shape = shape; }
	glm::vec3 direction() { return _direction; }
	glm::vec3 origin() { return _origin; }

private:
	glm::vec3 _origin;
	glm::vec3 _direction;

	float _t;

	Mesh* _shape;
};