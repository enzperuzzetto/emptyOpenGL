#pragma once

#include "mesh.h"

class Cube : public Mesh
{
public:
	Cube(float size = 1);
	~Cube() {};

	void update(glm::mat4& mat) override;
};