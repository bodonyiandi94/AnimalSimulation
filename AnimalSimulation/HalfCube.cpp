#include "HalfCube.h"

HalfCube::HalfCube():
	Mesh(PRIMITIVE_TYPE_TRIANGLES, DRAW_TYPE_SIMPLE)
{
	m_vertices = {
		glm::vec3(-0.5, -0.5,  0.5),
		glm::vec3(0.5, -0.5,  0.5),
		glm::vec3(-0.5,  0.5,  0.5),

		glm::vec3(0.5, -0.5,  0.5),
		glm::vec3(0.5,  0.5,  0.5),
		glm::vec3(-0.5,  0.5,  0.5),

		glm::vec3(0.5,  -0.5,  0.5),
		glm::vec3(0.5,  -0.5,  -0.5),
		glm::vec3(0.5,  0.5,  0.5),

		glm::vec3(0.5,  -0.5,  -0.5),
		glm::vec3(0.5,  0.5,  -0.5),
		glm::vec3(0.5,  0.5,  0.5),

		glm::vec3(-0.5,  -0.5,  0.5),
		glm::vec3(-0.5,  -0.5,  -0.5),
		glm::vec3(-0.5,  0.5,  -0.5),

		glm::vec3(-0.5,  -0.5,  0.5),
		glm::vec3(-0.5,  0.5,  -0.5),
		glm::vec3(-0.5,  0.5,  0.5),

		glm::vec3(-0.5,  -0.5,  0.5),
		glm::vec3(-0.5,  -0.5,  -0.5),
		glm::vec3(0.5,  -0.5,  -0.5),

		glm::vec3(-0.5,  -0.5,  0.5),
		glm::vec3(0.5,  -0.5,  -0.5),
		glm::vec3(0.5,  -0.5,  0.5),

		glm::vec3(-0.5,  0.5,  -0.5),
		glm::vec3(-0.5,  0.5,  0.5),
		glm::vec3(0.5,  0.5,  0.5),

		glm::vec3(-0.5,  0.5,  -0.5),
		glm::vec3(0.5,  0.5,  0.5),
		glm::vec3(0.5,  0.5,  -0.5),
	};

	generateNormals();
	buildMesh();
}

HalfCube::~HalfCube()
{
}