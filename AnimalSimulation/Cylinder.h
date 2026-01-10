#ifndef H__CYLINDER
#define H__CYLINDER

#include "Mesh.h"

class Cylinder : public Mesh
{
public:
	Cylinder(const unsigned &detail, const unsigned& circles);
	~Cylinder();

private:
	glm::vec3 generateVertex(const float& phi, const unsigned& hanyadik);
	void generateVertices();

private:
	unsigned m_detail, m_circles;
};

#endif //!H__CYLINDER