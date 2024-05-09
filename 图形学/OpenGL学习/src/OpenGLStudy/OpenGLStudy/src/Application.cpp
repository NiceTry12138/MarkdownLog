#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

const int gWidth = 640;
const int gHeight = 480;

void render() {
	glBegin(GL_TRIANGLES);
	glVertex2f(-0.5f, -0.5f);
	glVertex2f(0.0f, 0.5f);
	glVertex2f(0.5f, -0.5f);
	glEnd();
}

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
		std::cout << "Sharder Compile " << (inType == GL_VERTEX_SHADER ? "vertex sharder" : "fragment sharder") << " Faild" << std::endl;
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
		std::cout << "Compile Shader Faild: Can't Open File" << std::endl;
		return GL_ZERO;
	}

	std::string vertextShaderSrouce((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());
	ifs.close();

	ifs.open(fragmentShaderFilePath, std::ios::in);
	if (!ifs.is_open()) {
		std::cout << "Compile Shader Faild: Can't Open File" << std::endl;
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

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(gWidth, gHeight, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	if (GLEW_OK != glewInit()) {
		std::cout << "Error: glewInit Faild" << std::endl;
	}

	std::cout << glGetString(GL_VERSION) << std::endl;

	float positions[6] = {
		-0.5f, -0.5f,
		 0.0f,  0.5f,
		 0.5f, -0.5f
	};

	unsigned int buffer;
	glGenBuffers(1, &buffer);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);


	float positions1[6] = {
		-0.5f,  0.5f,
		 0.0f, -0.5f,
		 0.5f,  0.5f
	};
	unsigned int buffer1;
	glGenBuffers(1, &buffer1);
	glBindBuffer(GL_ARRAY_BUFFER, buffer1);
	glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions1, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	GLuint shader = CreateShaderWithFile("src/Vertex.vert", "src/Fragment.frag");
	glUseProgram(shader);

	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		glClear(GL_COLOR_BUFFER_BIT);

		//render();

		glEnableVertexAttribArray(0);

		glBindBuffer(GL_ARRAY_BUFFER, buffer1);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glBindBuffer(GL_ARRAY_BUFFER, buffer);
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	glDeleteProgram(shader);
	glfwTerminate();
	return 0;
}