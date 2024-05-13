#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"

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

		VertexArray va;

		VertexBuffer vb(positions, sizeof(float) * 2 * 4);

		VertexBufferLayout layout;
		layout.Push<float>(2);

		va.AddBuffer(vb, layout);

		IndexBuffer ibo(indeices, 6);

		auto shader = Shader("src/Vertex.vert", "src/Fragment.frag");

		// 清除所有绑定关系
		va.Unbind();
		shader.Unbind();
		vb.UnBind();
		ibo.Unbind();

		GLfloat r = 0.0f;
		GLfloat increment = 0.05f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			glClear(GL_COLOR_BUFFER_BIT);

			va.Bind();
			ibo.Bind();

			r += increment;
			shader.Bind();
			shader.SetUniform4f("u_Color", r, .5f, .5f, 1.0f);

			if (r > 1.0f || r < 0.0f) {
				increment *= -1;
			}

			GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}
	glfwTerminate();
	return 0;
}