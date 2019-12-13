#include "meshController.h"
#include "cube.h"
#include "line.h"

MeshController::MeshController() :
	_cube(new Cube(0.2)),
	_line(new Line()),
	_renderLine(false)
{
}

MeshController::~MeshController()
{
}

void MeshController::init()
{
	_cube->init();
	_line->init();

	_shader.loadFromFiles(DATA_DIR"/Shaders/simple.vert", DATA_DIR"/Shaders/simple.frag");

	_cube->setShader(_shader);
	_line->setShader(_shader);
}

void MeshController::render(const glm::mat4& vp)
{
	_cube->render(vp);
	if (_renderLine)
		_line->render(vp);
}

void MeshController::update(const glm::mat4& mat)
{
	_cube->update(mat);
	_line->update(mat);
}
