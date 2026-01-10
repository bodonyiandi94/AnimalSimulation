#ifndef H_OBJECT
#define H_OBJECT

#include "Mesh.h"
#include "Transform.h"
#include "Shader.h"

#include "gl/glm/glm.hpp"
#include "gl/glm/gtx/transform.hpp"
#include "gl/glm/gtc/type_ptr.hpp"
#include "gl/glm/gtc/matrix_inverse.hpp"

class Object:public Transform
{
public:
	Object();
	~Object();

	const glm::vec4& getFillColor() const;
	const glm::vec4& getHaloColor() const;
    Mesh* const& getMesh() const;
	Shader* getShader() const;

	void setFillColor(const glm::vec4&);
	void setHaloColor(const glm::vec4&);
	void setMesh(Mesh* const&);
	void setShader(Shader *shader);

private:
	Mesh* m_mesh;
	glm::vec4 m_fillColor, m_haloColor;
	Shader* m_shader;
};

#endif // !H_OBJECT
