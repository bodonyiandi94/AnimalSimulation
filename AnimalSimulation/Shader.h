#ifndef H__SHADER
#define H__SHADER

#include "gl/glew.h"

#include <fstream>
#include <sstream>

class Shader
{
public:
	Shader(const std::string& filePath);
	~Shader();

	void use();
	GLuint getUniformLocation(const std::string& uniformName) const;

private:
	void compile();

private:
	std::string m_filePath;
	GLuint m_program;
};

#endif // !H__SHADER
