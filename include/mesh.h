#pragma once
#include <vector>
#include <glm\glm.hpp>

#include "opengl.h"

class Shader;

class Mesh
{
public:
	Mesh() :
		_isInitialized(false),
		_modelMatrix(glm::mat4(1.f)),
		_shader(nullptr)
	{
	}

	virtual ~Mesh();

	void setShader(Shader& shader) { _shader = &shader; }

	virtual bool init();
	virtual void render(glm::mat4 VP);
	virtual void update(glm::mat4& mat) {};


protected:
	virtual void draw();

	bool _isInitialized;

	unsigned int _vao;
	unsigned int _ebo;
	unsigned int _vbo;

	glm::mat4 _modelMatrix;

	Shader* _shader;

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