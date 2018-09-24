#pragma once

class Program
{
private:
	unsigned int m_RendererID;
public:
	Program(const char* vertexShaderFilePath, const char* fragmentShaderFilePath);
	~Program();

	void Bind() const;
	void Unbind() const;
	inline const unsigned int GetRendererID() { return m_RendererID; }
};