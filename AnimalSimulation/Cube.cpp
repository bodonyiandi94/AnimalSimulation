#include "Cube.h"

Cube::Cube():
	Mesh(PRIMITIVE_TYPE_TRIANGLES, DRAW_TYPE_SIMPLE)
{
	m_vertices = {

		glm::vec3(0.5,  -0.5,  0.5),
		glm::vec3(0.5,  -0.5,  -0.5),
		glm::vec3(0.5,  0.5,  0.5),

		glm::vec3(0.5,  -0.5,  -0.5),
		glm::vec3(0.5,  0.5,  -0.5),
		glm::vec3(0.5,  0.5,  0.5),

		glm::vec3(-0.5,  -0.5,  -0.5),
		glm::vec3(0.5,  0.5,  -0.5),
		glm::vec3(0.5,  -0.5,  -0.5),

		glm::vec3(-0.5,  -0.5,  -0.5),
		glm::vec3(-0.5,  0.5,  -0.5),
		glm::vec3(0.5,  0.5,  -0.5),

		glm::vec3(-0.5,  -0.5,  0.5),
		glm::vec3(-0.5,  0.5,  -0.5),
		glm::vec3(-0.5,  -0.5,  -0.5),

		glm::vec3(-0.5,  -0.5,  0.5),
		glm::vec3(-0.5,  0.5,  0.5),
		glm::vec3(-0.5,  0.5,  -0.5),

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

	for (int i = 0; i < m_vertices.size(); i+=3)
	{
		std::swap(m_vertices[i + 1], m_vertices[i + 2]);
	}

	generateNormals();
	buildMesh();
}

Cube::~Cube()
{
}