#include "Shader.h"
#include <fstream>
#include <iostream>

Shader::Shader(const std::string& vertexFile, const std::string& fragmentFile)
{
	m_VertexFilePath = vertexFile;
	m_FragmentFilePath = fragmentFile;

	m_ShaderId = CreateShaderWithFile();
}

Shader::~Shader()
{
	glDeleteProgram(m_ShaderId);
}

void Shader::Bind() const
{
	glUseProgram(m_ShaderId);
}

void Shader::Unbind() const
{
	glUseProgram(0);
}

void Shader::SetUniform4f(const std::string& name, float v0, float v1, float v2, float v3)
{
	GLint location = GetUniformLocation(name);
	GL_CALL(glUniform4f(location, v0, v1, v2, v3));
}

GLuint Shader::GetUniformLocation(const std::string& name)
{
	if (m_Locations.find(name) != m_Locations.end()) {
		return m_Locations[name];
	}

	GLint location = -1;
	GL_CALL(location = glGetUniformLocation(m_ShaderId, "u_Color"));
	if (location == -1) {
		std::cout << "Warning: uniform " << name << " doesn't exist" << std::endl;
	}

	m_Locations[name] = location;	// ���û�ҵ� ��һֱ�Ҳ��� ���Բ��� else �ж�
	return location;
}

GLuint Shader::CreateShaderWithFile() {
	std::ifstream ifs;
	ifs.open(m_VertexFilePath, std::ios::in);
	if (!ifs.is_open()) {
		std::cout << "Compile Shader Fail: Can't Open File" << std::endl;
		return GL_ZERO;
	}

	std::string vertextShaderSrouce((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	ifs.open(m_FragmentFilePath, std::ios::in);
	if (!ifs.is_open()) {
		std::cout << "Compile Shader Fail: Can't Open File" << std::endl;
		return GL_ZERO;
	}

	std::string fragmentShaderSource((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	return CreateShader(vertextShaderSrouce, fragmentShaderSource);
}

GLuint Shader::CreateShader(const std::string& vertexSource, const std::string& fragmentSource)
{
	GLuint program = glCreateProgram();
	GLuint vs = CompileShader(vertexSource, GL_VERTEX_SHADER);
	GLuint fs = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);

	glAttachShader(program, vs);	// �󶨶�����ɫ��
	glAttachShader(program, fs);	// ��Ƭ����ɫ��
	glLinkProgram(program);			// ���ӳ��򣬽�������ɫ���ϲ�Ϊһ����ִ�еĳ���
	glValidateProgram(program);		// ��֤��������Ƿ�����ڵ�ǰ�� OpenGL ״̬��ִ��

	glDeleteShader(fs);				// ɾ����ɫ������ ��Ϊһ����ɫ�������ӵ����������ɫ���Ĵ����Ѿ������ӵ������У����Կ��԰�ȫ��ɾ����ɫ������
	glDeleteShader(vs);

	return program;
}

GLuint Shader::CompileShader(const std::string& source, GLenum inType)
{
	GLuint id = glCreateShader(inType);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Shader ������
	GLint result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		// alloca ��ջ�������ڴ棬����Ҫ free ����������������Զ��ͷ�
		char* msg = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, msg);
		std::cout << "Shader Compile " << (inType == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader") << " Fail" << std::endl;
		std::cout << msg << std::endl;

		glDeleteShader(id);
		return GL_ZERO;
	}

	return id;
}
