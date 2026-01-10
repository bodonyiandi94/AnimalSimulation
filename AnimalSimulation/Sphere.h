#ifndef H__SPHERE
#define H__SPHERE

#include "Mesh.h"

class Sphere :public Mesh
{
public:
	Sphere(const unsigned &detail);
	~Sphere();
	glm::vec3 generateVertex(const float& phi, const float& theta);

private:
	void generateVertices();

private:
	unsigned m_detail;
};

#endif // !H__SPHERE
