#ifndef H__MESH
#define H__MESH

#include "gl/glew.h"
#include "gl/glm/glm.hpp"
#include "gl/glm/gtx/transform.hpp"
#include "gl/glm/gtc/type_ptr.hpp"
#include "gl/glm/gtc/matrix_inverse.hpp"

#include <vector>
#include <unordered_map>

enum PrimitiveType
{
	PRIMITIVE_TYPE_LINES,
	PRIMITIVE_TYPE_LINE_STRIP,
	PRIMITIVE_TYPE_LINE_LOOP,
	PRIMITIVE_TYPE_TRIANGLES,
	PRIMITIVE_TYPE_TRIANGLE_STRIP,
	PRIMITIVE_TYPE_TRIANGLE_FAN,
};

enum DrawType
{
	DRAW_TYPE_SIMPLE,
	DRAW_TYPE_INDEXED,
};

class Mesh
{
public:
	Mesh(PrimitiveType primitiveType, DrawType drawType);
	virtual ~Mesh();
	void draw() const;

protected:
	void generateNormals();
	void buildMesh();
	
protected:
	PrimitiveType m_primitiveType;
	DrawType m_drawType;
	GLuint m_vbo, m_vao, m_ibo;
	std::vector<glm::vec3> m_vertices, m_normals;
	std::vector<GLuint> m_indices;
};

#endif // !H__MESH
