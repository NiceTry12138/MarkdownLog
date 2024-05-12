#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"

static GLuint CompiledShader(const std::string& source, GLenum inType) {
	GLuint id = glCreateShader(inType);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Shader 错误处理
	GLint result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		// alloca 在栈上申请内存，不需要 free ，在作用域结束后自动释放
		char* msg = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, msg);
		std::cout << "Shader Compile " << (inType == GL_VERTEX_SHADER ? "vertex shader" : "fragment shader") << " Fail" << std::endl;
		std::cout << msg << std::endl;
		
		glDeleteShader(id);
		return GL_ZERO;
	}

	return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	GLuint program = glCreateProgram();
	GLuint vs = CompiledShader(vertexShader, GL_VERTEX_SHADER);
	GLuint fs = CompiledShader(fragmentShader, GL_FRAGMENT_SHADER);

	glAttachShader(program, vs);	// 绑定顶点着色器
	glAttachShader(program, fs);	// 绑定片段着色器
	glLinkProgram(program);			// 链接程序，将所有着色器合并为一个可执行的程序
	glValidateProgram(program);		// 验证程序对象是否可以在当前的 OpenGL 状态下执行

	glDeleteShader(fs);				// 删除着色器对象 因为一旦着色器被链接到程序对象，着色器的代码已经被链接到程序中，所以可以安全地删除着色器对象
	glDeleteShader(vs);

	return program;
}

static GLuint CreateShaderWithFile(const std::string& vertexShaderFilePath, const std::string& fragmentShaderFilePath) {
	std::ifstream ifs;
	ifs.open(vertexShaderFilePath, std::ios::in);
	if (!ifs.is_open()) {
		std::cout << "Compile Shader Fail: Can't Open File" << std::endl;
		return GL_ZERO;
	}

	std::string vertextShaderSrouce((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	ifs.open(fragmentShaderFilePath, std::ios::in);
	if (!ifs.is_open()) {
		std::cout << "Compile Shader Fail: Can't Open File" << std::endl;
		return GL_ZERO;
	}

	std::string fragmentShaderSource((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	return CreateShader(vertextShaderSrouce, fragmentShaderSource);
}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);					// 设置 OpenGL 主版本为 3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);					// 设置 OpenGL 主版本为 3
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// 设置 OpenGL 为 核心

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(gWidth, gHeight, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	if (GLEW_OK != glewInit()) {
		std::cout << "Error: glewInit Faild" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	GLuint vao;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	{
		float positions[] = {
			-0.5f, -0.5f,
			 0.5f, -0.5f,
			 0.5f, 0.5f,
			-0.5f,  0.5f,
		};

		GLuint indeices[] = {
			0, 1, 2,
			2, 3, 0
		};

		VertexBuffer vb(positions, sizeof(float) * 2 * 4);

		GL_CALL(glEnableVertexAttribArray(0));
		GL_CALL(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0));

		IndexBuffer ibo(indeices, 6);

		GLuint shader = CreateShaderWithFile("src/Vertex.vert", "src/Fragment.frag");
		glUseProgram(shader);

		GLint location = -1;
		GL_CALL(location = glGetUniformLocation(shader, "u_Color"));
		GL_CALL(glUniform4f(location, 1, 1, 0, 0));

		// 清除所有绑定关系
		glBindVertexArray(0);
		glUseProgram(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


		GLfloat r = 0.0f;
		GLfloat increment = 0.05f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);

			//glDrawArrays(GL_TRIANGLES, 0, 6);
			//GL_CLEAR_ERROR;
			//glDrawElements(GL_TRIANGLES, 6, GL_UNIFORM, 0);
			//GL_CHECK_ERROR; 

			glBindVertexArray(vao);
			ibo.Bind();

			r += increment;
			GL_CALL(glUseProgram(shader));
			GL_CALL(glUniform4f(location, r, .5f, .5f, 1.0f));

			if (r > 1.0f || r < 0.0f) {
				increment *= -1;
			}

			GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
		glDeleteProgram(shader);
	}
	glfwTerminate();
	return 0;
}