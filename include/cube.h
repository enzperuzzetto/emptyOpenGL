#include "mesh.h"

class Cube : public Mesh
{
public:
	Cube() : Mesh(), mWidth(1) { init(); }
	Cube(float width) : Mesh(), mWidth(width) { init(); }

private:
	void init();
	
	float mWidth;
};