#ifndef H__CONE
#define H__CONE

#include "Mesh.h"

class Cone :public Mesh
{
public:
	Cone(const unsigned &detail, const unsigned& circles);
	~Cone();

private:
	glm::vec3 generateVertex(const float& phi, const unsigned& hanyadik);
	void generateVertices();

private:
	unsigned m_detail, m_circles;
};

#endif // !H__CONE
