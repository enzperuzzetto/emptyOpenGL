#include "cube.h"

using namespace glm;

Cube::Cube(float size)
{
	mVertices.push_back(Vertex(vec3(0.f, 0.f, 0.f)));
	mVertices.push_back(Vertex(vec3(0.f, size, 0.f)));
	mVertices.push_back(Vertex(vec3(size, size, 0.f)));
	mVertices.push_back(Vertex(vec3(size, 0.f, 0.f)));
	mVertices.push_back(Vertex(vec3(0.f, 0.f, size)));
	mVertices.push_back(Vertex(vec3(0.f, size, size)));
	mVertices.push_back(Vertex(vec3(size, size, size)));
	mVertices.push_back(Vertex(vec3(size, 0.f, size)));

	mIndices = {
		0,1,2,2,3,0,
		4,5,6,6,7,4,
		3,2,6,6,7,3,
		0,1,5,5,4,0,
		1,5,6,6,2,1,
		0,4,7,7,3,0
	};
}
