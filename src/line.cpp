#include "line.h"
#include "shader.h"

#include <glm\gtc\matrix_transform.hpp>

constexpr float LINE_LENGHT = 100.f;

Line::Line(glm::vec3 pos, glm::vec3 dir)
{
	mVertices.push_back(Vertex(pos));
	mVertices.push_back(Vertex(pos + dir * LINE_LENGHT));

	mIndices = { 0,1 };
}

void Line::update(const glm::mat4& mat)
{
	_modelMatrix = mat;
}

void Line::draw()
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

	glDrawElements(GL_LINES, mIndices.size(), GL_UNSIGNED_INT, nullptr);

	glDisableVertexAttribArray(vertex_pos_loc);

	if (vertex_color_loc >= 0) glDisableVertexAttribArray(vertex_color_loc);

	glBindVertexArray(0);

	checkError();
}