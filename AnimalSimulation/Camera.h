#ifndef H__CAMERA
#define H__CAMERA

#include "gl/glm/glm.hpp"
#include "gl/glm/gtx/transform.hpp"
#include "gl/glm/gtc/type_ptr.hpp"
#include "gl/glm/gtc/matrix_inverse.hpp"

#include "Transform.h"

class Camera:public Transform
{
public:
	Camera();
	~Camera();

	const float& getFov() const;
	const float& getAspectRatio() const;
	const float& getNearZ() const;
	const float& getFarZ() const;

	glm::mat4 getView() const;
	glm::mat4 getProjection() const;
	glm::mat4 getViewProjection() const;

	void setFov(const float&);
	void setAspectRatio(const float&);
	void setNearZ(const float&);
	void setFarZ(const float&);

private:
	float m_fov, m_aspectRatio, m_nearZ, m_farZ;
};

#endif // !H__CAMERA
