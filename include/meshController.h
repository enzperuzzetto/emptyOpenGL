#pragma once
#include "shader.h"

#include <glm\glm.hpp>

class Cube;
class Line;

class MeshController
{
public:
	MeshController();
	~MeshController();

	void init();
	void render(const glm::mat4& vp);
	void update(const glm::mat4& mat);
	void setRenderLine(bool activate) { _renderLine = activate; }

private:
	Cube* _cube;
	Line* _line;
	Shader _shader;
	bool _renderLine;
};