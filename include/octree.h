#pragma once

#include <vector>
#include <array>
#include <glm\glm.hpp>

class AABB;
class Mesh;
class Ray;

class Octree
{
public:
	//Octree();
	Octree(AABB* bv, std::vector<Mesh*> list);
	//Octree(BoundingVolume* bv);
	~Octree();

	void buildTree();
	bool intersect(Ray& ray);
	void render(const glm::mat4& vp);

	Octree* m_parent;
	AABB* m_bv;
private:
	Octree* createNode(AABB& bv, std::vector<Mesh*> list);

	std::array<Octree*, 8> m_octant;

	std::vector<Mesh*> m_objects;

	unsigned char m_activeNodes = 0;

	bool m_treeReady = false;
	bool m_treeBuild = false;
	const int MIN_SIZE = 1;// 100 * SCALE_FACTOR;
	const int MIN_NB_OBJECT = 5;
};