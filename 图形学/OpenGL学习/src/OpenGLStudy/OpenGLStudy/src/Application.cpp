#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>

#include "Util.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "VertexBufferLayout.h"
#include "Shader.h"
#include "Renderer.h"
#include "Texture.h"

#include "vendor/glm/glm.hpp"
#include "vendor/glm/gtc/matrix_transform.hpp"

int main(void)
{
	GLFWwindow* window = nullptr;

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

	// 设定宽高是 640, 480 比例是 4:3 
	//glm::mat4 proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);

	glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));
	glm::mat4 mvp = proj * view;

	{
		//float positions[] = {
		//	-0.5f, -0.5f, 0.0f, 0.0f,
		//	 0.5f, -0.5f, 1.0f, 0.0f,
		//	 0.5f,  0.5f, 1.0f, 1.0f,
		//	-0.5f,  0.5f, 0.0f, 1.0f,
		//};		
		float positions[] = {
			100.0f, 100.0f, 0.0f, 0.0f,
			700.0f, 100.0f, 1.0f, 0.0f,
			700.0f, 400.0f, 1.0f, 1.0f,
			100.0f, 400.0f, 0.0f, 1.0f,
		};

		GLuint indeices[] = {
			0, 1, 2,
			2, 3, 0
		};

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		VertexArray va;

		VertexBuffer vb(positions, sizeof(float) * 4 * 4);

		VertexBufferLayout layout;
		layout.Push<float>(2);	// 前两个是 顶点
		layout.Push<float>(2);	// 后两个是 UV 坐标

		va.AddBuffer(vb, layout);

		IndexBuffer ibo(indeices, 6);

		Texture texture("res/textures/ChernoLogo.png");
		texture.Bind(0);

		auto shader = Shader("res/shader/Vertex.vert", "res/shader/Fragment.frag");
		shader.Bind();
		shader.SetUniform1i("u_Texture", 0);
		shader.SetUniformMat4f("u_MVP", mvp);

		// 清除所有绑定关系
		va.Unbind();
		vb.UnBind();
		ibo.Unbind();
		shader.Unbind();

		Renderer render;

		GLfloat r = 0.0f;
		GLfloat increment = 0.05f;
		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			/* Render here */
			//glClear(GL_COLOR_BUFFER_BIT);
			render.Clear();

			va.Bind();
			ibo.Bind();

			r += increment;
			//shader.Bind();
			//shader.SetUniform4f("u_Color", r, .5f, .5f, 1.0f);

			if (r > 1.0f || r < 0.0f) {
				increment *= -1;
			}

			//GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
			render.Draw(va, ibo, shader);

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}
	}
	glfwTerminate();
	return 0;
}