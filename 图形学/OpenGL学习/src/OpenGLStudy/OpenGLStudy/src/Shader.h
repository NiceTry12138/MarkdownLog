#pragma once
#include "Util.h"
#include <string>
#include <unordered_map>

#include "vendor/glm/glm.hpp"

class Shader
{
private:
	std::string m_VertexFilePath;
	std::string m_FragmentFilePath;
	GLuint m_ShaderId;

	// 缓存 uniform 的 Location
	std::unordered_map<std::string, GLint> m_Locations;

public:
	Shader(const std::string& vertexFile, const std::string& fragmentFile);
	Shader();
	~Shader();

	void Init(const std::string& vertexFile, const std::string& fragmentFile);

	void Bind() const;
	void Unbind() const;

	void SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3);
	void SetUniform1i(const std::string& name, int v0);
	void SetUniformMat4f(const std::string& name, const glm::mat4& inMat4);
		
private:
	GLint GetUniformLocation(const std::string& name);
	GLuint CreateShaderWithFile();
	GLuint CreateShader(const std::string& vertexSource, const std::string& fragmentSource);
	GLuint CompileShader(const std::string& source, GLenum inType);
};

