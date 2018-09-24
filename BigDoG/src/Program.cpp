#include "Program.h"
#include "Renderer.h"
#include <string>
#include <iostream>
#include <fstream>

void ReadShaderFile(const char* filePath, std::string& result) {
	std::ifstream inputFileStream(filePath);
	result.clear();
	result.assign((std::istreambuf_iterator<char>(inputFileStream)),
		(std::istreambuf_iterator<char>()));
}

unsigned int CompileShader(unsigned int type, const std::string& source)
{
	GLCall(unsigned int id = glCreateShader(type));
	const char* src = source.c_str();
	GLCall(glShaderSource(id, 1, &src, nullptr));
	GLCall(glCompileShader(id));

	int result;
	GLCall(glGetShaderiv(id, GL_COMPILE_STATUS, &result));

	if (result == GL_FALSE)
	{
		int length;
		GLCall(glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length));
		char* message = (char*)alloca(length * sizeof(char));
		GLCall(glGetShaderInfoLog(id, length, &length, message));
		std::cout << "Failed to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment") << " shader" << std::endl;
		std::cout << message << std::endl;
		GLCall(glDeleteShader(id));
		return 0;
	}

	return id;
}

int CreateProgram(const std::string& vertexShader, const std::string& fragmentShader)
{
	GLCall(unsigned int program = glCreateProgram());
	GLCall(unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader));
	GLCall(unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader));

	GLCall(glAttachShader(program, vs));
	GLCall(glAttachShader(program, fs));
	GLCall(glLinkProgram(program));
	GLCall(glValidateProgram(program));

	GLCall(glDeleteShader(vs));
	GLCall(glDeleteShader(fs));

	return program;
}

Program::Program(const char * vertexShaderFilePath, const char * fragmentShaderFilePath)
{
	std::string vertexShader, fragmentShader;
	ReadShaderFile(vertexShaderFilePath, vertexShader);
	ReadShaderFile(fragmentShaderFilePath, fragmentShader);
	m_RendererID = CreateProgram(vertexShader, fragmentShader);
}

Program::~Program()
{
	GLCall(glDeleteProgram(m_RendererID));
}

void Program::Bind() const
{
	GLCall(glUseProgram(m_RendererID));
}

void Program::Unbind() const
{
	GLCall(glUseProgram(0));
}
