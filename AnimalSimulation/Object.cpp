#include "Object.h"

Object::Object() :
	m_mesh(NULL),
	m_shader(NULL)
{

}

Object::~Object()
{

}

const glm::vec4& Object::getFillColor() const
{
	return m_fillColor;
}

const glm::vec4& Object::getHaloColor() const
{
	return m_haloColor;
}

Mesh* const& Object::getMesh() const
{
	return m_mesh;
}

Shader* Object::getShader() const
{
	return m_shader;
}


void Object::setFillColor(const glm::vec4& fillColor)
{
	m_fillColor = fillColor;
}

void Object::setHaloColor(const glm::vec4& haloColor)
{
	m_haloColor = haloColor;
}

void Object::setMesh(Mesh* const& mesh)
{
	m_mesh = mesh;
}

void Object::setShader(Shader* shader)
{
	m_shader = shader;
}