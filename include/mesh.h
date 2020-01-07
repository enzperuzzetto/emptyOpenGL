#pragma once
#include <vector>
#include <glm\glm.hpp>

#include "opengl.h"
#include "boundingVolume.h"

class Shader;

class Mesh
{
public:
	Mesh() :
		_isInitialized(false),
		_modelMatrix(glm::mat4(1.f)),
		_shader(nullptr),
		_bv(nullptr)
	{
	}

	virtual ~Mesh();

	void setShader(Shader& shader) { _shader = &shader; }
	bool inColision() { return _bv->inColision; }
	void setInColision(bool value) { _bv->inColision = value; }
	BoundingVolume* getBoundingVolume() { return _bv; }

	virtual bool init();
	virtual void render(glm::mat4 VP);
	virtual void update(glm::mat4& mat) {};

	virtual bool intersect(Ray& ray);
	virtual bool intersect(Mesh& mesh);

protected:
	virtual void draw();

	bool _isInitialized;
	bool _drawBV = true;

	unsigned int _vao;
	unsigned int _ebo;
	unsigned int _vbo;

	glm::mat4 _modelMatrix;

	Shader* _shader;
	BoundingVolume* _bv;

	struct Vertex
	{
		Vertex(glm::vec3 pos, glm::vec3 col = glm::vec3(1.f, 1.f, 1.f), glm::vec2 tex = glm::vec2(1.f, 1.f), glm::vec3 n = glm::vec3(0.f)) :
			position(pos), color(col), texcoord(tex), normal(n) {};
			
		glm::vec3 position;
		glm::vec3 color;
		glm::vec2 texcoord;
		glm::vec3 normal;
	};

	std::vector<Vertex> mVertices;
	std::vector<unsigned int> mIndices;
};