#include "cube.h"

using namespace glm;

void Cube::init()
{
	Vertex v0 = Vertex(vec3(0.f, 0.f, 0.f));
	Vertex v1 = Vertex(vec3(mWidth, 0.f, 0.f));
	Vertex v2 = Vertex(vec3(0.f, mWidth, 0.f));
	/*
	Vertex v3 = Vertex(vec3(mWidth, mWidth, 0.f));
	Vertex v4 = Vertex(vec3(0.f, 0.f, mWidth));
	Vertex v5 = Vertex(vec3(mWidth, 0.f, mWidth));
	Vertex v6 = Vertex(vec3(0.f, mWidth, mWidth));
	Vertex v7 = Vertex(vec3(mWidth, mWidth, mWidth));
	*/

	mVertices.push_back(v0);
	mVertices.push_back(v1);
	mVertices.push_back(v2);
	/*
	mVertices.push_back(v3);
	mVertices.push_back(v4);
	mVertices.push_back(v5);
	mVertices.push_back(v6);
	mVertices.push_back(v7);
	

	uvec3 fac0 = uvec3(0,1,3);
	uvec3 fac1 = uvec3(3,2,0);
	uvec3 fac2 = uvec3(4,5,7);
	uvec3 fac3 = uvec3(7,6,4);
	uvec3 fac4 = uvec3(4,0,2);
	uvec3 fac5 = uvec3(2,6,4);
	uvec3 fac6 = uvec3(1,5,7);
	uvec3 fac7 = uvec3(7,3,1);
	uvec3 fac8 = uvec3(2,3,7);
	uvec3 fac9 = uvec3(7,6,2);
	uvec3 fac10 = uvec3(4,0,1);
	uvec3 fac11 = uvec3(1,5,4);

	mFaces = { fac0, fac1, fac2,fac3, fac4, fac5, fac6,fac7,fac8,fac9,fac10,fac11 };
	*/

	mFaces = { uvec3(0,1,2) };
}
