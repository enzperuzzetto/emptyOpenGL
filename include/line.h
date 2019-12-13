#pragma once
#include "mesh.h"

class Line : public Mesh
{
public:
	Line(glm::vec3 pos = glm::vec3(0.f), glm::vec3 dir = glm::vec3(0.f, 0.f, 1.f));
	~Line() {};

	void update(const glm::mat4& viewMatrix) override;

private:
	void draw() override;
};