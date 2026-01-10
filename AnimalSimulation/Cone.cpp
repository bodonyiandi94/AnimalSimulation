#include "Cone.h"

Cone::Cone(const unsigned &detail, const unsigned& circles) :
	Mesh(PRIMITIVE_TYPE_TRIANGLES, DRAW_TYPE_SIMPLE),
	m_detail(detail),
	m_circles(circles)
{
	generateVertices();
	generateNormals();
	buildMesh();
}

Cone::~Cone()
{
	
}

glm::vec3 Cone::generateVertex(const float& phi, const unsigned& hanyadik)
{
	float percent = (1.0f / m_circles)*hanyadik;

	return glm::vec3(
		percent*cos(phi),
		(percent),
		-sin(phi)*percent
		);
}

void Cone::generateVertices()
{
	float step = glm::two_pi<float>() / m_detail;

	for (unsigned circleId = 0; circleId < m_circles; circleId ++)
	{
		for (unsigned circleRad = 0; circleRad < m_detail; circleRad++)
		{
			float phi = circleRad*step;

			m_vertices.push_back(generateVertex(phi, circleId));
			m_vertices.push_back(generateVertex(phi + step, circleId));
			m_vertices.push_back(generateVertex(phi + step, circleId + 1));

			m_vertices.push_back(generateVertex(phi, circleId));
			m_vertices.push_back(generateVertex(phi + step, circleId + 1));
			m_vertices.push_back(generateVertex(phi, circleId + 1));
		}
	}
}