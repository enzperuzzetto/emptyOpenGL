#include "mesh.h"

#include <fstream>
#include <limits>
#include <glm/gtx/norm.hpp >
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"

using namespace glm;

Mesh::~Mesh()
{
	if (isInitialized)
	{
		glDeleteBuffers(1, &mVertexBufferId);
		glDeleteBuffers(1, &mIndexBufferId);
	}
}

bool Mesh::load(const std::string& filename)
{
	std::string ext = filename.substr(filename.size() - 3, 3);

	// loader of 3d object file
	if (ext == "obj" || ext == "OBJ")
	{
		return loadOBJ(filename);
	}
	
	std::cerr << "Mesh: extension \'" << ext << "\' not supported." << std::endl;
	return false;
}

void Mesh::computeNormals()
{
	for (unsigned int i = 0; i < mVertices.size(); i++)
	{
		mVertices[i].normal = vec3(0.f);
		mVertices[i].tangente = vec3(0.f);
		mVertices[i].bitangente = vec3(0.f);
	}

	for (unsigned int i = 0; i < mFaces.size(); i++)
	{
		vec3 p1 = mVertices[mFaces[i].x].position;
		vec3 p2 = mVertices[mFaces[i].y].position;
		vec3 p3 = mVertices[mFaces[i].z].position;

		vec3 u = p2 - p1;
		vec3 v = p3 - p1;
		vec3 n = cross(u, v);

		mVertices[mFaces[i].x].normal += n;
		mVertices[mFaces[i].y].normal += n;
		mVertices[mFaces[i].z].normal += n;

		vec2 p1_tex = mVertices[mFaces[i].x].texcoord;
		vec2 p2_tex = mVertices[mFaces[i].y].texcoord;
		vec2 p3_tex = mVertices[mFaces[i].z].texcoord;

		vec2 st1 = p2_tex - p1_tex;
		vec2 st2 = p3_tex - p1_tex;

		mat2 tex = { st1, st2 };

		mat2x3 pos = mat2x3();
		pos[0] = u;
		pos[1] = v;

		mat2x3 tb = pos * inverse(tex);
		vec3 tan = tb[0];
		vec3 bitan = tb[1];

		mVertices[mFaces[i].x].tangente += tan;
		mVertices[mFaces[i].y].tangente += tan;
		mVertices[mFaces[i].z].tangente += tan;

		mVertices[mFaces[i].x].bitangente += bitan;
		mVertices[mFaces[i].y].bitangente += bitan;
		mVertices[mFaces[i].z].bitangente += bitan;
	}

	// Gram-Schmidt tangente, bitangente
	for (unsigned int i = 0; i < mVertices.size(); i++)
	{
		mVertices[i].tangente = mVertices[i].tangente - dot(mVertices[i].normal, mVertices[i].tangente) * mVertices[i].normal;
		mVertices[i].bitangente = mVertices[i].bitangente - dot(mVertices[i].normal, mVertices[i].bitangente) * mVertices[i].normal - dot(mVertices[i].tangente, mVertices[i].bitangente) * mVertices[i].tangente / l2Norm(mVertices[i].tangente);

		mVertices[i].normal = normalize(mVertices[i].normal);
		mVertices[i].tangente = normalize(mVertices[i].tangente);
		mVertices[i].bitangente = normalize(mVertices[i].bitangente);
	}
}

void Mesh::initVBA()
{
	glGenBuffers(1, &mVertexBufferId);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(),&mVertices[0].position, GL_STATIC_DRAW);

	glGenBuffers(1, &mIndexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uvec3) * mFaces.size(), &mFaces[0], GL_STATIC_DRAW);

	glGenVertexArrays(1, &mVAO);

	isInitialized = true;
}

void Mesh::draw(const Shader& shader)
{
	glBindVertexArray(mVAO);
	glBindBuffer(GL_ARRAY_BUFFER, mVertexBufferId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mIndexBufferId);

	int vertex_loc = shader.getAttribLocation("vtx_position");
	glVertexAttribPointer(vertex_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)&mVertices[0].position);
	glEnableVertexAttribArray(vertex_loc);

	//give all attribut you nedd in vert shader
	int color_loc = shader.getAttribLocation("vtx_color");
	if (color_loc >= 0)
	{
		glVertexAttribPointer(color_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)&mVertices[0].color);
		glEnableVertexAttribArray(color_loc);
	}
	
	glDrawElements(GL_TRIANGLES, 3 * mFaces.size(), GL_UNSIGNED_INT, 0);

	glDisableVertexAttribArray(vertex_loc);
	if (color_loc >= 0) glDisableVertexAttribArray(color_loc);

	checkError();
}
	

#include <ObjFormat/ObjFormat.h>

bool Mesh::loadOBJ(const std::string& filename)
{
	ObjMesh* pRawObjMesh = ObjMesh::LoadFromFile(filename);

	if (!pRawObjMesh)
	{
		std::cerr << "Mesh::loadObj: error loading file " << filename << "." << std::endl;
		return false;
	}

	// Makes sure we have an indexed face set
	ObjMesh* pObjMesh = pRawObjMesh->createIndexedFaceSet(Obj::Options(Obj::AllAttribs | Obj::Triangulate));
	delete pRawObjMesh;
	pRawObjMesh = 0;

	// copy vertices
	mVertices.resize(pObjMesh->positions.size());

	for (std::size_t i = 0; i < pObjMesh->positions.size(); ++i)
	{
		mVertices[i] = Vertex(vec3(pObjMesh->positions[i].x, pObjMesh->positions[i].y, pObjMesh->positions[i].z));

		if (!pObjMesh->texcoords.empty())
			mVertices[i].texcoord = vec2(pObjMesh->texcoords[i].x, pObjMesh->texcoords[i].y);

		if (!pObjMesh->normals.empty())
			mVertices[i].normal = vec3(pObjMesh->normals[i].x, pObjMesh->normals[i].y, pObjMesh->normals[i].z);
	}

	// copy faces
	for (std::size_t smi = 0; smi < pObjMesh->getNofSubMeshes(); ++smi)
	{
		const ObjSubMesh* pSrcSubMesh = pObjMesh->getSubMesh(smi);

		mFaces.reserve(pSrcSubMesh->getNofFaces());

		for (std::size_t fid = 0; fid < pSrcSubMesh->getNofFaces(); ++fid)
		{
			ObjConstFaceHandle srcFace = pSrcSubMesh->getFace(fid);
			mFaces.push_back(vec3(srcFace.vPositionId(0), srcFace.vPositionId(1), srcFace.vPositionId(2)));
		}
	}

	if (pObjMesh->normals.empty())
		computeNormals();

	return true;
}
