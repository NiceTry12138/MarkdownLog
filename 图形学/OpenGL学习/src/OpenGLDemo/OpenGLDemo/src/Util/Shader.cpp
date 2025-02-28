#include "Shader.h"
#include <fstream>
#include <sstream>
#include <iostream>

GLuint Shader::s_CurrentBindShader = GL_ZERO;

void checkCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

void Shader::NewInit(const std::string& inVertexFile, const std::string& inFragmentFile)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(inVertexFile);
		fShaderFile.open(inFragmentFile);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// shader Program
	m_ShaderID = glCreateProgram();
	glAttachShader(m_ShaderID, vertex);
	glAttachShader(m_ShaderID, fragment);
	glLinkProgram(m_ShaderID);
	checkCompileErrors(m_ShaderID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::Init(const std::string& inVertexFile, const std::string& inFragmentFile)
{
	if (m_ShaderID != GL_ZERO) 
	{
		DeleteShader();
	}

	std::ifstream ifs;
	ifs.open(inVertexFile, std::ios::in);
	if (!ifs.is_open()) {
		std::cout << "Compile Shader Fail: Can't Open File" << std::endl;
		return;
	}

	std::string vertextShaderSrouce((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	ifs.open(inFragmentFile, std::ios::in);
	if (!ifs.is_open()) {
		std::cout << "Compile Shader Fail: Can't Open File" << std::endl;
		return;
	}

	std::string fragmentShaderSource((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	m_ShaderID = CreateShader(vertextShaderSrouce, fragmentShaderSource);
}

Shader::~Shader()
{
	GL_CALL(glDeleteProgram(m_ShaderID));
}

void Shader::SetUniform1i(const std::string& inName, const int& value)
{
	// ��ֹʹ��ʱ�����Ȱ� ��Ϊ�˱���ÿ�����ö��� ʹ�� s_CurrentBindShader ��¼��״̬
	Bind();
	auto location = GetShaderLocation(inName);
	if (location == -1)
	{
		return;
	}
	GL_CALL(glUniform1i(location, value));
}

void Shader::SetUniform4f(const std::string& inName, float v0, float v1, float v2, float v3)
{
	Bind();
	GLint location = GetShaderLocation(inName);
	if (location == -1)
	{
		return;
	}
	GL_CALL(glUniform4f(location, v0, v1, v2, v3));
}

void Shader::SetUniformMat4f(const std::string& inName, const glm::mat4& inMat4)
{
	Bind();
	GLint location = GetShaderLocation(inName);
	if (location == -1)
	{
		return;
	}
	GL_CALL(glUniformMatrix4fv(location, 1, GL_FALSE, &inMat4[0][0]));
}

void Shader::Bind()
{
	if (s_CurrentBindShader == m_ShaderID)
	{
		return;
	}
	GL_CALL(glUseProgram(m_ShaderID));
	s_CurrentBindShader = m_ShaderID;
}

void Shader::UnBind()
{
	if (s_CurrentBindShader != GL_ZERO)
	{
		GL_CALL(glUseProgram(0));
		s_CurrentBindShader = GL_ZERO;
	}
}

bool Shader::IsValid()
{
	return m_ShaderID == GL_ZERO;
}

void Shader::DeleteShader()
{
	GL_CALL(glDeleteShader(m_ShaderID));
}

GLint Shader::GetShaderLocation(const std::string& inName)
{
	if (m_locationMap.find(inName) != m_locationMap.end())
	{
		return m_locationMap[inName];
	}

	GLint result = -1;
	result = glGetUniformLocation(m_ShaderID, inName.c_str());

	if (result == -1) {
		std::cout << "can't find Shader Location with Name " << inName << std::endl;
	}
	
	// shader ���ݲ���䣬ͬһ�� shader û����� name �Ĳ۾�һ��û��
	m_locationMap[inName] = result;

	return result;
}

GLuint Shader::CreateShader(const std::string& vertexSource, const std::string& fragmentSource)
{
	GLuint program = glCreateProgram();
	
	GLuint vs = CompileShader(vertexSource, GL_VERTEX_SHADER);
	GLuint fs = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);

	GL_CALL(glAttachShader(program, vs));			// �� vs �󶨵� program ��
	GL_CALL(glAttachShader(program, fs));			// �� fs �󶨵� program ��

	GL_CALL(glLinkProgram(program));					// ���ӳ��򣬽�������ɫ���ϲ�Ϊһ����ִ�еĳ���
	GL_CALL(glValidateProgram(program));				// ��֤�����Ƿ����ִ��

	GL_CALL(glDeleteShader(vs));						// ɾ����ɫ������һ�����ӳɹ���ô�����Ѿ����ӵ��������ˣ�����ɾ����ɫ������
	GL_CALL(glDeleteShader(fs));
	
	return program;
}

GLuint Shader::CompileShader(const std::string& inSource, GLenum inType)
{
	GLuint shaderId = glCreateShader(inType);

	const char* source = inSource.c_str();
	GL_CALL(glShaderSource(shaderId, 1, &source, nullptr));			// ���� nullptr ��ʾ��ȡ�����ַ�������
	GL_CALL(glCompileShader(shaderId));

	GLint errorId;
	GL_CALL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &errorId));
	if (errorId == GL_FALSE)
	{
		// �������
		GLint length = 0;
		GL_CALL(glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length));
		
		char* msg = (char*)alloca(length);
		GL_CALL(glGetShaderInfoLog(shaderId, length, &length, msg));

		std::cout << "Shader Compile " << (inType == GL_VERTEX_SHADER ? "Vertex Shader" : "Fragment Shader") << " Failed" << std::endl;
		std::cout << msg << std::endl;

		GL_CALL(glDeleteShader(shaderId));
		return GL_ZERO;
	}

	return shaderId;
}

