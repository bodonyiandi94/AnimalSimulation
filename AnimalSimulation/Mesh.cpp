#include "Mesh.h"

struct hashFunc
{
	size_t operator()(const glm::vec3 &k) const
	{
		size_t h1 = std::hash<float>()(k.x);
		size_t h2 = std::hash<float>()(k.y);
		size_t h3 = std::hash<float>()(k.z);
		return (h1 ^ (h2 << 1)) ^ h3;
	}
};

struct equalsFunc
{
	bool operator()(const glm::vec3& lhs, const glm::vec3& rhs) const
	{
		return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
	}
};


Mesh::Mesh(PrimitiveType primitiveType, DrawType drawType) :
	m_primitiveType(primitiveType),
	m_drawType(drawType),
	m_vbo(0),
	m_vao(0),
	m_ibo(0)
{
	glGenBuffers(1, &m_vbo);
	glGenVertexArrays(1, &m_vao);
	glGenBuffers(1, &m_ibo);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &m_vao);
	glDeleteBuffers(1, &m_vbo);
	glDeleteBuffers(1, &m_ibo);
}

void Mesh::generateNormals()
{
	for (unsigned i = 0;i < m_vertices.size() / 3;i++)
	{
		unsigned baseIndex = i * 3;
		glm::vec3 edges[2] = {
			m_vertices.at(baseIndex + 1) - m_vertices.at(baseIndex),
			m_vertices.at(baseIndex + 2) - m_vertices.at(baseIndex),
		};

		glm::vec3 normal = glm::normalize(glm::cross(edges[0], edges[1]));

		m_normals.push_back(normal);
		m_normals.push_back(normal);
		m_normals.push_back(normal);
	}

	std::unordered_map<glm::vec3, glm::vec3, hashFunc, equalsFunc> normalMap;
}

void Mesh::buildMesh()
{
	std::vector<unsigned char> bufferData;

	for (unsigned i = 0;i < m_vertices.size();i++)
	{
		bufferData.insert(bufferData.end(), (unsigned char*)glm::value_ptr(m_vertices[i]),
			(unsigned char*)glm::value_ptr(m_vertices[i]) + sizeof(glm::vec3));

		bufferData.insert(bufferData.end(), (unsigned char*)glm::value_ptr(m_normals[i]),
			(unsigned char*)glm::value_ptr(m_normals[i]) + sizeof(glm::vec3));
	}

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, bufferData.size(), &bufferData[0], GL_STREAM_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(m_vao);

	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), 0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2 * sizeof(glm::vec3), (void*)sizeof(glm::vec3));

	glBindVertexArray(0);
}

void Mesh::draw() const
{
	GLenum primitiveType = 0;
	switch (m_primitiveType)
	{
	case PRIMITIVE_TYPE_LINES:
		primitiveType = GL_LINES;
		break;
	case PRIMITIVE_TYPE_LINE_STRIP:
		primitiveType = GL_LINE_STRIP;
		break;
	case PRIMITIVE_TYPE_LINE_LOOP:
		primitiveType = GL_LINE_LOOP;
		break;
	case PRIMITIVE_TYPE_TRIANGLES:
		primitiveType = GL_TRIANGLES;
		break;
	case PRIMITIVE_TYPE_TRIANGLE_STRIP:
		primitiveType = GL_TRIANGLE_STRIP;
		break;
	case PRIMITIVE_TYPE_TRIANGLE_FAN:
		primitiveType = GL_TRIANGLE_FAN;
		break;
	default:
		break;
	}

	glBindVertexArray(m_vao);

	switch (m_drawType)
	{
	case DRAW_TYPE_SIMPLE:
		glDrawArrays(primitiveType, 0, m_vertices.size());
		break;
	case DRAW_TYPE_INDEXED:
		glDrawElements(primitiveType, m_indices.size(), GL_UNSIGNED_INT, 0);
		break;
	default:
		break;
	}

	glBindVertexArray(0);
}