#include "octree.h"
#include "boundingVolume.h"
#include "ray.h"
#include "mesh.h"

using namespace std;

Octree::Octree(AABB* bv, vector<Mesh*> list) :
	m_parent(nullptr),
	m_bv(bv),
	m_octant(array<Octree*, 8>()),
	m_objects(list)
{
}

Octree::~Octree()
{
}

void Octree::buildTree()
{
	m_bv->init();
	if (m_objects.size() < MIN_NB_OBJECT)
		return;

	if(glm::all(glm::lessThan(m_bv->radius * 2.f, glm::vec3(MIN_SIZE))))
		return;

	array<AABB*,8> octant =  array<AABB*,8>();
	glm::vec3 offset(0.f);
	glm::vec3 step = m_bv->radius * 0.5f;
	for (int i = 0; i < 8; i++)
	{
		offset.x = (i & 1) ? step.x : -step.x;
		offset.y = (i & 2) ? step.y : -step.y;
		offset.z = (i & 4) ? step.z : -step.z;
		octant[i] = new AABB(m_bv->center + offset, step);
	}

	vector<vector<Mesh*>> octantList = vector<vector<Mesh*>>(8);
	for (int i = 0; i < 8; i++)
		octantList[i] = vector<Mesh*>();

	vector<int> delist = vector<int>();

	for (int n=0; n<m_objects.size(); n++)
	{
		for (int i = 0; i < 8; i++)
		{
			if (octant[i]->contain(*m_objects[n]->getBoundingVolume()))
			{
				octantList[i].push_back(m_objects[n]);
				delist.push_back(n);
				break;
			}
		}
	}

	if (delist.size() == m_objects.size())
		m_objects.clear();
	else
	{
		for (int i = 0; i < delist.size(); i++)
			m_objects.erase(m_objects.begin() + (delist[i] - i));
	}

	for (int oct = 0; oct < 8; oct++)
	{
		if (!octantList[oct].empty())
		{
			m_octant[oct] = createNode(*octant[oct], octantList[oct]);
			m_activeNodes |= (1 << oct);
			m_octant[oct]->buildTree();
		}
	}

	m_treeBuild = true;
	m_treeReady = true;
}

bool Octree::intersect(Ray& ray)
{
	if(!m_bv->intersect(ray))
		return false;
	else
	{
		float tmin = FLT_MAX;
		bool haveIntersectObject = false;
		for (int i = 0; i < m_objects.size(); i++)
		{
			if (m_objects[i]->intersect(ray))
			{
				haveIntersectObject = true;
				if (tmin > ray.t())
				{
					ray.setShape(m_objects[i]);
					tmin = ray.t();
				}
			}
		}

		if (!haveIntersectObject)
			ray.setT(FLT_MAX);

		array<Ray, 8> rays = array<Ray, 8>();
		for (int i = 0; i < 8; i++)
			rays[i] = Ray(ray); //copy

		for (unsigned char flags = m_activeNodes, index = 0; flags > 0; flags >>= 1, index++)
		{
			if ((flags & 1) == 1)
			{
				m_octant[index]->intersect(rays[index]);
			}
		}
		
		for (int i = 0; i < 8; i++)
		{
			if (ray.t() > rays[i].t())
			{
				ray.setShape(rays[i].shape());
				ray.setT(rays[i].t());
			}
		}
		
		return ray.foundIntersection();
	}
}

void Octree::render(const glm::mat4& vp)
{
	m_bv->render(vp);
	for (unsigned char flags = m_activeNodes, index = 0; flags > 0; flags >>= 1, index++)
	{
		if ((flags & 1) == 1)
		{
			m_octant[index]->render(vp);
		}
	}
}

Octree* Octree::createNode(AABB& bv, std::vector<Mesh*> list)
{
	if (list.empty())
		return nullptr;

	Octree* octree = new Octree(&bv, list);
	octree->m_parent = this;
	return octree;
}
