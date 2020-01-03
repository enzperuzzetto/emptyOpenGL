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

	float tmin = 0.0f;
	float tmax = FLT_MAX;
	for (int i = 0; i < 3; i++) {

		if (std::fabs(ray_direction[i]) < std::numeric_limits<float>::epsilon()) {
			// Ray is parallel to slab. No hit if origin not within slab
			if (ray_origin[i] < min[i] || ray_origin[i] > max[i]) return 0;
		}
		else {
			// Compute intersection t value of ray with near and far plane of slab
			float ood = 1.0f / ray_direction[i];
			float t1 = (min[i] - ray_origin[i]) * ood;
			float t2 = (max[i] - ray_origin[i]) * ood;
			// Make t1 be intersection with near plane, t2 with far plane
			if (t1 > t2) std::swap(t1, t2);
			// Compute the intersection of slab intersection intervals
			if (t1 > tmin) tmin = t1;
			if (t2 > tmax) tmax = t2;
			// Exit with no collision as soon as slab intersection becomes empty
			if (tmin > tmax) return 0;
		}
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
