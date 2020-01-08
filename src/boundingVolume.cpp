#include "boundingVolume.h"
#include "ray.h"
#include "shader.h"

#include <math.h>
#include <glm/gtc/type_ptr.hpp>

AABB::AABB(glm::vec3 pos, glm::vec3 rad) :
	center(pos),
	init_center(pos),
	radius(rad),
	init_radius(rad)
{
}

void AABB::init()
{
	box_render = AABB_Render(center, radius);
	box_render.init();
}

void AABB::update(const glm::mat4& mv)
{
	glm::vec3 translate = glm::vec3(mv[3]);
	for (int i = 0; i < 3; i++)
	{
		center[i] = translate[i];
		radius[i] = 0.f;
		for (int j = 0; j < 3; j++)
		{
			center[i] += mv[j][i] * init_center[j];
			radius[i] += fabsf(mv[j][i]) * init_radius[j];
		}
	}
	
	box_render.update(*this);
}

void AABB::render(const glm::mat4& vp)
{
	box_render.render(vp, inColision);
}

bool AABB::contain(BoundingVolume& bv)
{
	AABB* box = dynamic_cast<AABB*>(&bv);
	if (box != nullptr)
	{
		glm::vec3 min = center - radius;
		glm::vec3 max = center + radius;
		glm::vec3 boxMin = box->center - box->radius;
		glm::vec3 boxMax = box->center + box->radius;

		if (glm::all(glm::lessThanEqual(min, boxMin)) && glm::all(glm::greaterThanEqual(max, boxMax)))
			return true;
		else
			return false;
	}
	else
		return false;
}

bool AABB::intersect(BoundingVolume& bv)
{
	AABB* box = dynamic_cast<AABB*>(&bv);
	if (box != nullptr)
	{
		if (fabsf(center.x - box->center.x) > (radius.x + box->radius.x)) return false;
		if (fabsf(center.y - box->center.y) > (radius.y + box->radius.y)) return false;
		if (fabsf(center.z - box->center.z) > (radius.z + box->radius.z)) return false;
		return true;
	}
	return false;
}

bool AABB::intersect(Ray& ray)
{
	glm::vec3 ray_origin = ray.origin();
	glm::vec3 ray_direction = ray.direction();
	
	glm::vec3 min = center - radius;
	glm::vec3 max = center + radius;
	
	float t1 = (min.x - ray_origin.x) / ray_direction.x;
	float t2 = (max.x - ray_origin.x) / ray_direction.x;
	float t3 = (min.y - ray_origin.y) / ray_direction.y;
	float t4 = (max.y - ray_origin.y) / ray_direction.y;
	float t5 = (min.z - ray_origin.z) / ray_direction.z;
	float t6 = (max.z - ray_origin.z) / ray_direction.z;

	float tmin = fmaxf(fmaxf(fminf(t1, t2), fminf(t3, t4)), fminf(t5, t6));
	float tmax = fminf(fminf(fmaxf(t1, t2), fmaxf(t3, t4)), fmaxf(t5, t6));

	// if tmax < 0, ray (line) is intersecting AABB, but whole AABB is behing us
	if (tmax < 0) {
		return false;
	}

	// if tmin > tmax, ray doesn't intersect AABB
	if (tmin > tmax) {
		return false;
	}

	if (tmin < 0.f) {
		ray.setT(tmax);
	}
	ray.setT(tmin);

	return true;
}

AABB_Render::AABB_Render(glm::vec3 pos, glm::vec3 rad) :
	_shader(new Shader())
{
	mVertices = {
		glm::vec3(pos.x - rad.x, pos.y - rad.y, pos.z - rad.z),
		glm::vec3(pos.x - rad.x, pos.y + rad.y, pos.z - rad.z),
		glm::vec3(pos.x + rad.x, pos.y + rad.y, pos.z - rad.z),
		glm::vec3(pos.x + rad.x, pos.y - rad.y, pos.z - rad.z),
		glm::vec3(pos.x - rad.x, pos.y - rad.y, pos.z + rad.z),
		glm::vec3(pos.x - rad.x, pos.y + rad.y, pos.z + rad.z),
		glm::vec3(pos.x + rad.x, pos.y + rad.y, pos.z + rad.z),
		glm::vec3(pos.x + rad.x, pos.y - rad.y, pos.z + rad.z)
	};

	mIndices = {
		0,1,1,2,2,3,3,0,
		0,4,1,5,2,6,3,7,
		4,5,5,6,6,7,7,4
	};
}

bool AABB_Render::init()
{
	if (_isInitialized)
		return _isInitialized;

	// vbo position
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	// copy the data from host's RAM to GPU's video memory:
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);

	unsigned int ebo;
	glGenBuffers(1, &ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mIndices.size(), mIndices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);

	_shader->loadFromFiles(DATA_DIR"/Shaders/aabb.vert", DATA_DIR"/Shaders/aabb.frag");

	_shader->activate();

	int vertex_pos_loc = _shader->getAttribLocation("vtx_position");
	glVertexAttribPointer(vertex_pos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), 0);
	glEnableVertexAttribArray(vertex_pos_loc);

	_shader->deactivate();

	glBindVertexArray(0);
	_isInitialized = true;

	return _isInitialized;
}

void AABB_Render::update(const AABB& box)
{
	mVertices = {
		glm::vec3(box.center.x - box.radius.x, box.center.y - box.radius.y, box.center.z - box.radius.z),
		glm::vec3(box.center.x - box.radius.x, box.center.y + box.radius.y, box.center.z - box.radius.z),
		glm::vec3(box.center.x + box.radius.x, box.center.y + box.radius.y, box.center.z - box.radius.z),
		glm::vec3(box.center.x + box.radius.x, box.center.y - box.radius.y, box.center.z - box.radius.z),
		glm::vec3(box.center.x - box.radius.x, box.center.y - box.radius.y, box.center.z + box.radius.z),
		glm::vec3(box.center.x - box.radius.x, box.center.y + box.radius.y, box.center.z + box.radius.z),
		glm::vec3(box.center.x + box.radius.x, box.center.y + box.radius.y, box.center.z + box.radius.z),
		glm::vec3(box.center.x + box.radius.x, box.center.y - box.radius.y, box.center.z + box.radius.z)
	};

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);
	glBindVertexArray(0);
}

void AABB_Render::render(glm::mat4 MVP, bool inColision)
{
	if (!_isInitialized)
	{
		std::cerr << "AABB_Render not initialized !\n";
		return;
	}

	_shader->activate();
	glUniformMatrix4fv(_shader->getUniformLocation("mvp"), 1, GL_FALSE, glm::value_ptr(MVP));
	if (inColision)
		glUniform3fv(_shader->getUniformLocation("in_color"), 1, glm::value_ptr(colision));
	else
		glUniform3fv(_shader->getUniformLocation("in_color"), 1, glm::value_ptr(nocolision));

	draw();

	_shader->deactivate();
}

void AABB_Render::draw()
{
	glBindVertexArray(_vao);

	glDrawElements(GL_LINES, mIndices.size(), GL_UNSIGNED_INT, nullptr);

	glBindVertexArray(0);

	checkError();
}
