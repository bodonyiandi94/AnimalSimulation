#include "Transform.h"

Transform::Transform() :
	m_parent(NULL),
	m_scale(1),
	m_useCustomTransform(false)
{

}

glm::mat4 Transform::getModel() const
{
	glm::mat4 model;
	
	if (m_useCustomTransform)
	{
		model = m_transform;
	} 
	else {
		model = glm::translate(m_position)*
			/*
			glm::rotate(m_rotation.z, glm::vec3(0, 0, 1))*
			glm::rotate(m_rotation.y, glm::vec3(0, 1, 0))*
			glm::rotate(m_rotation.x, glm::vec3(1, 0, 0))*
			*/
			glm::rotate(m_rotation.x, glm::vec3(1, 0, 0))*
			glm::rotate(m_rotation.y, glm::vec3(0, 1, 0))*
			glm::rotate(m_rotation.z, glm::vec3(0, 0, 1))*
			glm::scale(m_scale);
	}

	if (m_parent != NULL)
	{
		model = m_parent->getModel()*model;
	}

	return model;
}

const glm::vec3& Transform::getPosition() const
{
	return m_position;
}

const glm::vec3& Transform::getRotation() const
{
	return m_rotation;
}

const glm::vec3& Transform::getScale() const
{
	return m_scale;
}

const glm::mat4& Transform::getTransform() const
{
	return m_transform;
}

Transform* const& Transform::getParent() const
{
	return m_parent;
}

void Transform::setPosition(const glm::vec3& position)
{
	m_position = position;
	m_useCustomTransform = false;
}

void Transform::setRotation(const glm::vec3& rotation)
{
	m_rotation = rotation;
	m_useCustomTransform = false;
}
void Transform::setScale(const glm::vec3& scale)
{
	m_scale = scale;
	m_useCustomTransform = false;
}

void Transform::setTransform(const glm::mat4& transform)
{
	m_transform = transform;
	m_useCustomTransform = true;
}

void Transform::lookAt(const glm::vec3& target)
{
	glm::vec3 position = getWorldPosition();
	glm::vec3 dir = target - m_position;

	m_rotation.z = glm::atan(dir.y, glm::sqrt(dir.x * dir.x + dir.z * dir.z)) + glm::half_pi<float>();
	m_rotation.y = -glm::atan(dir.z, dir.x);
	m_rotation.x = 0.0f;
}

void Transform::setParent(Transform* const& parent)
{
	m_parent = parent;
}

glm::vec3 Transform::getUp()
{
	return glm::normalize(glm::vec3(getModel()*glm::vec4(0, 1, 0, 0)));
}

glm::vec3 Transform::getRight()
{
	return glm::normalize(glm::vec3(getModel()*glm::vec4(1, 0, 0, 0)));
}

glm::vec3 Transform::getForward()
{
	return glm::normalize(glm::vec3(getModel()*glm::vec4(0, 0, -1, 0)));
}

glm::vec3 Transform::getWorldPosition() const
{
	return glm::vec3(getModel() * glm::vec4(m_position, 1.0f));
}