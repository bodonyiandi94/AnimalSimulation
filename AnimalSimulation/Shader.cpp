#include "Shader.h"

#include "sdl2/sdl.h"

Shader::Shader(const std::string& filePath):
	m_filePath(filePath)
{
	m_program = glCreateProgram();

	compile();
}

Shader::~Shader()
{
	glDeleteProgram(m_program);
}

void Shader::use()
{
	glUseProgram(m_program);
}

GLuint Shader::getUniformLocation(const std::string& uniformName) const
{
	return glGetUniformLocation(m_program, uniformName.c_str());
}

void Shader::compile()
{
	GLuint vs = glCreateShader(GL_VERTEX_SHADER);
	GLuint fs = glCreateShader(GL_FRAGMENT_SHADER);

	std::fstream fin;
	std::stringstream stream;

	fin.open(m_filePath + ".vs");
	if (!fin.good())
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,"Cannot open vertex shader %s.vs",m_filePath.c_str());

	stream << fin.rdbuf();

	std::string vertexShader = stream.str();

	stream = std::stringstream();
	fin.close();

	fin.open(m_filePath + ".fs");
	if (!fin.good())
		SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "Cannot open fragment shader %s.fs", m_filePath.c_str());

	stream << fin.rdbuf();
	std::string fragmentShader = stream.str();
	fin.close();

	const GLchar* vsSource[] = { vertexShader.c_str() };
	const GLchar* fsSource[] = { fragmentShader.c_str() };

	glShaderSource(vs, 1, vsSource, NULL);
	glShaderSource(fs, 1, fsSource, NULL);

	glCompileShader(vs);
	glCompileShader(fs);

	GLint compileStatus;
	glGetShaderiv(vs, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE)
	{
		GLint logSize = 0;
		glGetShaderiv(vs, GL_INFO_LOG_LENGTH, &logSize);

		char* log = new char[logSize];
		glGetShaderInfoLog(vs, logSize, &logSize, log);
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Error compiling vertex shader, reason: \n %s", log);

		delete[] log;
	}

	glGetShaderiv(fs, GL_COMPILE_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE)
	{
		GLint logSize = 0;
		glGetShaderiv(fs, GL_INFO_LOG_LENGTH, &logSize);

		char* log = new char[logSize];
		glGetShaderInfoLog(fs, logSize, &logSize, log);
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Error compiling fragment shader, reason: \n %s", log);

		delete[] log;
	}

	glAttachShader(m_program, vs);
	glAttachShader(m_program, fs);

	glLinkProgram(m_program);

	glGetProgramiv(m_program, GL_LINK_STATUS, &compileStatus);

	if (compileStatus == GL_FALSE)
	{
		GLint logSize = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &logSize);

		char* log = new char[logSize];
		glGetProgramInfoLog(m_program, logSize, &logSize, log);
		SDL_LogError(SDL_LOG_CATEGORY_RENDER, "Error linking program, reason: \n %s", log);

		delete[] log;
	}

	glDetachShader(m_program, vs);
	glDetachShader(m_program, fs);

	glDeleteShader(vs);
	glDeleteShader(fs);
}