#include "Sphere.h"
#include <iostream>

Sphere::Sphere(const unsigned &detail) :
	Mesh(PRIMITIVE_TYPE_TRIANGLES, DRAW_TYPE_SIMPLE),
	m_detail(detail)
{
	generateVertices();
	generateNormals();
	buildMesh();
}

Sphere::~Sphere()
{
}

glm::vec3 Sphere::generateVertex(const float& phi, const float& theta)
{
	return glm::vec3(
		cos(theta)*sin(phi),
		cos(phi),
		sin(theta)*sin(phi)
		);
}

void Sphere::generateVertices()
{
	float step = glm::pi<float>() / m_detail;

	for (int i = 0; i < m_detail; i++)
	{
		float phi=i*step;

		for (int j = 0; j<m_detail*2; j++)
		{
			float theta=j*step;

			glm::vec3 faces[2][3] =
			{
				{
					(generateVertex(phi + step, theta + step)),
					(generateVertex(phi + step, theta)),
					(generateVertex(phi, theta)),
				},

				{
					(generateVertex(phi + step, theta + step)),
					(generateVertex(phi, theta)),
					(generateVertex(phi, theta + step)),
				}
			};

			if (i > 0)
			{
				m_vertices.push_back(faces[0][0]);
				m_vertices.push_back(faces[0][1]);
				m_vertices.push_back(faces[0][2]);

				m_vertices.push_back(faces[1][0]);
				m_vertices.push_back(faces[1][1]);
				m_vertices.push_back(faces[1][2]);
			}
			else
			{
				m_vertices.push_back(faces[0][0]);
				m_vertices.push_back(faces[0][1]);
				m_vertices.push_back(faces[0][2]);
				
				//m_vertices.push_back(faces[1][0]);
				//m_vertices.push_back(faces[1][2]);
				//m_vertices.push_back(faces[1][1]);
				
			}
		}
	}
}