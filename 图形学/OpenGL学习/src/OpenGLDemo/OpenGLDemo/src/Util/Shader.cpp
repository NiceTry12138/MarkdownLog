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

void Shader::Init(const std::string& inVertexFile, const std::string& inFragmentFile, const std::string& inGeoFile)
{
	if (m_ShaderID != GL_ZERO) 
	{
		DeleteShader();
	}

	// 读取 顶点着色器
	std::ifstream ifs;
	ifs.open(inVertexFile, std::ios::in);
	if (!ifs.is_open()) {
		std::cout << "Compile Shader Fail: Can't Open File" << std::endl;
		return;
	}

	std::string vertextShaderSrouce((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	// 读取 片段着色器
	ifs.open(inFragmentFile, std::ios::in);
	if (!ifs.is_open()) {
		std::cout << "Compile Shader Fail: Can't Open File" << std::endl;
		return;
	}

	std::string fragmentShaderSource((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	// 读取 几何着色器
	std::string geoShaderSource;
	if (!inGeoFile.empty())
	{
		ifs.open(inGeoFile, std::ios::in);
		if (!ifs.is_open()) {
			std::cout << "Compile Shader Fail: Can't Open File" << std::endl;
			return;
		}

		geoShaderSource = std::string((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
		ifs.close();
	}

	m_ShaderID = CreateShader(vertextShaderSrouce, fragmentShaderSource, geoShaderSource);
}

Shader::~Shader()
{
	GL_CALL(glDeleteProgram(m_ShaderID));
}

// 在 Attach 之后，需要重新绑定 UBO、顶点属性、Uniform 变量
void Shader::AttachGeoShader(const std::string& inGeoShaderFile)
{
	std::ifstream ifs;
	ifs.open(inGeoShaderFile, std::ios::in);
	if (!ifs.is_open()) {
		std::cout << "Compile Shader Fail: Can't Open File" << std::endl;
		return;
	}

	std::string geoShaderSrouce((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	GLuint gs = CompileShader(geoShaderSrouce, GL_GEOMETRY_SHADER);

	GL_CALL(glAttachShader(m_ShaderID, gs));			// 将 vs 绑定到 program 上
	GL_CALL(glLinkProgram(m_ShaderID));					// 链接程序，将所有着色器合并为一个可执行的程序
	GL_CALL(glValidateProgram(m_ShaderID));				// 验证程序是否可以执行
	GL_CALL(glDeleteShader(gs));
}

void Shader::SetUniform1i(const std::string& inName, const int& value)
{
	// 防止使用时忘记先绑定 又为了避免每次设置都绑定 使用 s_CurrentBindShader 记录绑定状态
	Bind();
	auto location = GetShaderLocation(inName);
	if (location == -1)
	{
		return;
	}
	GL_CALL(glUniform1i(location, value));
}

void Shader::SetUniform1f(const std::string& inName, const float& value)
{
	Bind();
	GLint location = GetShaderLocation(inName);
	if (location == -1)
	{
		return;
	}
	GL_CALL(glUniform1f(location, value));
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

void Shader::SetUniform3f(const std::string& inName, float v0, float v1, float v2)
{
	Bind();
	GLint location = GetShaderLocation(inName);
	if (location == -1)
	{
		return;
	}
	GL_CALL(glUniform3f(location, v0, v1, v2));
}

void Shader::BindUBO(const std::string& inName, int inSlot)
{
	Bind();
	GLint location = glGetUniformBlockIndex(m_ShaderID, inName.c_str());
	if (location == -1) {
		std::cout << "Can't Find Uniform Buffer Object: " << inName << " In Shader" << std::endl;
		return;
	}
	GL_CALL(glUniformBlockBinding(m_ShaderID, location, inSlot));
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
	
	// shader 内容不会变，同一个 shader 没有这个 name 的槽就一定没有
	m_locationMap[inName] = result;

	return result;
}

GLuint Shader::CreateShader(const std::string& vertexSource, const std::string& fragmentSource, const std::string& geoSource)
{
	GLuint program = glCreateProgram();
	
	GLuint vs = CompileShader(vertexSource, GL_VERTEX_SHADER);
	GLuint fs = CompileShader(fragmentSource, GL_FRAGMENT_SHADER);

	GL_CALL(glAttachShader(program, vs));			// 将 vs 绑定到 program 上
	GL_CALL(glAttachShader(program, fs));			// 将 fs 绑定到 program 上

	GLuint gs = GL_ZERO;
	if (!geoSource.empty())
	{
		gs = CompileShader(geoSource, GL_GEOMETRY_SHADER);
		GL_CALL(glAttachShader(program, gs));			// 将 vs 绑定到 program 上
	}

	GL_CALL(glLinkProgram(program));					// 链接程序，将所有着色器合并为一个可执行的程序
	GL_CALL(glValidateProgram(program));				// 验证程序是否可以执行

	GL_CALL(glDeleteShader(vs));						// 删除着色器对象，一旦链接成功那么代码已经连接到程序中了，可以删除着色器对象
	GL_CALL(glDeleteShader(fs));
	GL_CALL(glDeleteShader(gs));

	return program;
}

GLuint Shader::CompileShader(const std::string& inSource, GLenum inType)
{
	GLuint shaderId = glCreateShader(inType);

	const char* source = inSource.c_str();
	GL_CALL(glShaderSource(shaderId, 1, &source, nullptr));			// 传入 nullptr 表示读取整个字符串数组
	GL_CALL(glCompileShader(shaderId));

	GLint errorId;
	GL_CALL(glGetShaderiv(shaderId, GL_COMPILE_STATUS, &errorId));
	if (errorId == GL_FALSE)
	{
		// 编译错误
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

