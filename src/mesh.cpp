#include "mesh.h"
#include "shader.h"

#include <glm/gtc/type_ptr.hpp>

bool Mesh::init()
{
	if (_isInitialized)
		return _isInitialized;

	// vbo position
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	// copy the data from host's RAM to GPU's video memory:
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * mVertices.size(), mVertices.data(), GL_DYNAMIC_DRAW);

	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mIndices.size(), mIndices.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &_vao);

	_isInitialized = true;

	return _isInitialized;
}

void Mesh::draw()
{
	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	
	int vertex_pos_loc = _shader->getAttribLocation("vtx_position");
	glVertexAttribPointer(vertex_pos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(vertex_pos_loc);
	
	int vertex_color_loc = _shader->getAttribLocation("vtx_color");
	if (vertex_color_loc >= 0)
	{
		glVertexAttribPointer(vertex_pos_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &mVertices[0].color);
		glEnableVertexAttribArray(vertex_pos_loc);
	}
	
	int vertex_normal_loc = _shader->getAttribLocation("vtx_normal");
	if (vertex_normal_loc >= 0)
	{
		glVertexAttribPointer(vertex_normal_loc, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), &mVertices[0].normal);
		glEnableVertexAttribArray(vertex_normal_loc);
	}
	
	int vertex_texcoord_loc = _shader->getAttribLocation("vtx_texcoord");
	if (vertex_texcoord_loc >= 0)
	{
		glVertexAttribPointer(vertex_texcoord_loc, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), &mVertices[0].texcoord);
		glEnableVertexAttribArray(vertex_texcoord_loc);
	}
	
	glDrawElements(GL_TRIANGLES, mIndices.size(), GL_UNSIGNED_INT, nullptr);
	
	glDisableVertexAttribArray(vertex_pos_loc);
	
	if (vertex_color_loc >= 0) glDisableVertexAttribArray(vertex_color_loc);
	if (vertex_normal_loc >= 0)  glDisableVertexAttribArray(vertex_normal_loc);
	if (vertex_texcoord_loc >= 0)  glDisableVertexAttribArray(vertex_texcoord_loc);
	
	glBindVertexArray(0);

	checkError();
}

void Mesh::render(glm::mat4 VP)
{
	if (!_isInitialized)
	{
		std::cerr << "mesh not initialized !\n";
		return;
	}

	_shader->activate();
	glm::mat4 mvp = VP * _modelMatrix;
	glUniformMatrix4fv(_shader->getUniformLocation("mvp"), 1, GL_FALSE, glm::value_ptr(mvp));

	draw();

	_shader->deactivate();
}