#pragma once
#include "CommonHead.h"

class Shader
{
public:
	void NewInit(const std::string& inVertexFile, const std::string& inFragmentFile);
	void Init(const std::string& inVertexFile, const std::string& inFragmentFile);
	~Shader();

	void SetUniform1i(const std::string& inName, const int& value);
	void SetUniform1f(const std::string& inName, const float& value);
	void SetUniform4f(const std::string& inName, float v0, float v1, float v2, float v3);
	void SetUniformMat4f(const std::string& inName, const glm::mat4& inMat4);
	void SetUniform3f(const std::string& inName, float v0, float v1, float v2);

	void Bind();
	void UnBind();

	bool IsValid();
	void DeleteShader();

protected:
	GLint GetShaderLocation(const std::string& inName);

	GLuint CreateShader(const std::string& vertexSource, const std::string& fragmentSource);
	GLuint CompileShader(const std::string& inSource, GLenum inType);

private:
	static GLuint s_CurrentBindShader;

	//GLuint CreateShaderWithFile(const std::string& vertexFile, const std::string& fragmentFile);
private:
	std::unordered_map<std::string, GLint> m_locationMap;	// 槽位映射
	GLuint m_ShaderID{ GL_ZERO };
};

