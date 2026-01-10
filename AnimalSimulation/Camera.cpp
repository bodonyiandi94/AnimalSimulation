#include "Camera.h"
#include "sdl2\SDL.h"

Camera::Camera() :
	m_fov(glm::half_pi<float>()),
	m_aspectRatio(1.0f),
	m_nearZ(0.0f),
	m_farZ(100.0f)
{

}

Camera::~Camera()
{

}

const float& Camera::getFov() const
{
	return m_fov;
}

const float& Camera::getAspectRatio() const
{
	return m_aspectRatio;
}

const float& Camera::getNearZ() const
{
	return m_nearZ;
}

const float& Camera::getFarZ() const
{
	return m_farZ;
}

glm::mat4 Camera::getView() const
{
	glm::mat4 model = getModel();
	glm::vec3 worldSize;
	worldSize.x = glm::length(model[0]);
	worldSize.y = glm::length(model[1]);
	worldSize.z = glm::length(model[2]);
	return glm::inverse(model*glm::scale(1.0f/worldSize));
}

glm::mat4 Camera::getProjection() const
{
	return glm::perspective(m_fov, m_aspectRatio, m_nearZ, m_farZ);
}

glm::mat4 Camera::getViewProjection() const
{
	return getProjection()*getView();
}


void Camera::setFov(const float& fov)
{
	m_fov = fov;
}

void Camera::setAspectRatio(const float& aspectRatio)
{
	m_aspectRatio = aspectRatio;
}

void Camera::setNearZ(const float& nearZ)
{
	m_nearZ = nearZ;
}

void Camera::setFarZ(const float& farZ)
{
	m_farZ = farZ;
}