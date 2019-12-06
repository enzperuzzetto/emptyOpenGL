#include <trackball.h>
#include <glm\gtc\constants.hpp>

using namespace glm;

Trackball::Trackball() :
	cur_pos(vec2(0.f)),
	last_pos(vec2(0.f)),
	_screenWidth(100),
	_screenHeight(100)
{
}

Trackball::Trackball(int width_screen, int height_screen) :
	cur_pos(vec2(0.f)),
	last_pos(vec2(0.f)),
	_screenWidth(width_screen),
	_screenHeight(height_screen)
{
}

Trackball::~Trackball()
{
}

void Trackball::start()
{
	arc_ball = true;
}

bool Trackball::trackRotate(const vec2 pos, float& angle, vec3& axis)
{
	cur_pos = pos;
	bool ok = false;

	if (cur_pos != last_pos)
	{
		vec3 va = computeArcballVector(cur_pos.x, cur_pos.y);
		vec3 vb = computeArcballVector(last_pos.x, last_pos.y);
		angle = acosf(min(1.0f, dot(va, vb)));
		axis = cross(va, vb);
		ok = true;
	}

	last_pos = cur_pos;
	return ok;
}

bool Trackball::trackTranslate(const glm::vec2 pos, glm::vec3& v)
{
	cur_pos = pos;
	bool ok = false;

	if (cur_pos != last_pos)
	{
		v = vec3(cur_pos.x - last_pos.x, cur_pos.y - last_pos.y, 0.f);
		ok = true;
	}

	last_pos = cur_pos;

	return ok;
}

void Trackball::end()
{
	arc_ball = false;
}

// https://en.wikibooks.org/wiki/OpenGL_Programming/Modern_OpenGL_Tutorial_Arcball
vec3 Trackball::computeArcballVector(float x, float y)
{
	vec3 P = vec3(1.0f * x / (float)_screenWidth * 2.f - 1.0f,	1.0f * y / (float)_screenHeight * 2.f - 1.0f, 0.f);
	P.y = -P.y;
	float OP_squared = P.x * P.x + P.y * P.y;
	if (OP_squared <= 1)
		P.z = sqrt(1 - OP_squared);
	else
		P = normalize(P);
	return P;
}

