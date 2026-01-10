#ifndef H__TRANSFORM
#define H__TRANSFORM

#include "gl/glm/glm.hpp"
#include "gl/glm/gtx/transform.hpp"
#include "gl/glm/gtc/type_ptr.hpp"
#include "gl/glm/gtc/matrix_inverse.hpp"

class Transform
{
public:
	Transform();

	glm::mat4 getModel() const;
	const glm::vec3& getPosition() const;
	const glm::vec3& getRotation() const;
	const glm::vec3& getScale() const;
	const glm::mat4& getTransform() const;
	Transform* const& getParent() const;
	glm::vec3 getUp();
	glm::vec3 getForward();
	glm::vec3 getRight();
	glm::vec3 getWorldPosition() const;

	void setPosition(const glm::vec3&);
	void setRotation(const glm::vec3&);
	void setScale(const glm::vec3&);
	void setTransform(const glm::mat4&);
	void setParent(Transform* const&);
	void lookAt(const glm::vec3&);

private:
	glm::vec3 m_position, m_rotation, m_scale;
	Transform* m_parent;
	glm::mat4 m_transform;
	bool m_useCustomTransform;
};

#endif // !H__TRANSFORM
