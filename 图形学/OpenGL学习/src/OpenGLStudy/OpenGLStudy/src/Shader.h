#pragma once
#include "Util.h"
#include <string>
#include <unordered_map>

class Shader
{
private:
	std::string m_VertexFilePath;
	std::string m_FragmentFilePath;
	GLuint m_ShaderId;

	// »º´æ uniform µÄ Location
	std::unordered_map<std::string, GLint> m_Locations;

public:
	Shader(const std::string& vertexFile, const std::string& fragmentFile);
	~Shader();

	void Bind() const;
	void Unbind() const;

	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);

private:
	GLuint GetUniformLocation(const std::string& name);
	GLuint CreateShaderWithFile();
	GLuint CreateShader(const std::string& vertexSource, const std::string& fragmentSource);
	GLuint CompileShader(const std::string& source, GLenum inType);
};

