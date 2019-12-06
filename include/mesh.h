#include <vector>
#include <glm/glm.hpp>

#include "opengl.h"


class Shader;

class Mesh
{
public:

	Mesh() :
		mVertices(std::vector<Vertex>()),
		mFaces(std::vector<glm::uvec3>())
	{};
	virtual ~Mesh();

	bool load(const std::string& filename);

	void computeNormals();
	void initVBA();

	virtual void draw(const Shader& shader);

protected:

	bool loadOBJ(const std::string& filename);

	struct Vertex
	{
		Vertex(const glm::vec3& pos = glm::vec3(0.f), const glm::vec3& n = glm::vec3(0.f)) :
			position(pos), normal(n), color(glm::vec4(0.6, 0.6, 0.6, 1.0)), texcoord(glm::vec2(0.f))
		{};

		glm::vec3 position;
		glm::vec4 color;
		glm::vec3 normal;
		glm::vec2 texcoord;
		glm::vec3 tangente;
		glm::vec3 bitangente;
	};

	std::vector<Vertex> mVertices;
	std::vector<glm::uvec3> mFaces;

	unsigned int mVertexBufferId;
	unsigned int mIndexBufferId;
	unsigned int mVAO;
	bool isInitialized;

};