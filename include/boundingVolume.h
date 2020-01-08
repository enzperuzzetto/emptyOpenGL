#pragma once

#include <glm\glm.hpp>
#include <vector>

class Ray;
class Shader;

class BoundingVolume
{
public:
	BoundingVolume() {};
	~BoundingVolume() {};

	virtual void init() = 0;
	virtual void update(const glm::mat4& mat) = 0;
	virtual void render(const glm::mat4& mvp) = 0;
	virtual bool contain(BoundingVolume& bv) = 0;
	virtual bool intersect(BoundingVolume& bv) = 0;
	virtual bool intersect(Ray& ray) = 0;

	bool inColision = false;
};

class AABB;

class AABB_Render
{
public:
	AABB_Render(glm::vec3 pos = glm::vec3(0.f), glm::vec3 rad = glm::vec3(1.f));
	~AABB_Render() {};

	bool init();
	void update(const AABB& box);
	void render(glm::mat4 VP, bool colision = false);

private:
	void draw();

	bool _isInitialized = false;

	unsigned int _vao;
	unsigned int _vbo;

	Shader* _shader;

	std::vector<glm::vec3> mVertices;
	std::vector<unsigned int> mIndices;
	glm::vec3 nocolision = glm::vec3(0.f, 1.f, 0.f);
	glm::vec3 colision = glm::vec3(1.f, 0.f, 0.f);
};

class AABB : public BoundingVolume
{
public:
	AABB(glm::vec3 pos = glm::vec3(0.f), glm::vec3 rad = glm::vec3(1.f));
	~AABB() {};

	void init();
	void update(const glm::mat4& mat);
	void render(const glm::mat4& mvp);
	bool contain(BoundingVolume& bv);
	bool intersect(BoundingVolume& bv);
	bool intersect(Ray& ray);

	glm::vec3 center;
	glm::vec3 radius;

private:
	AABB_Render box_render;

	glm::vec3 init_center;
	glm::vec3 init_radius;
};