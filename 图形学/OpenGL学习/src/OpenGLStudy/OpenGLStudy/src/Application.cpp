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

#include "vendor/imgui/imgui.h"
#include "vendor/imgui/imgui_impl_glfw.h"
#include "vendor/imgui/imgui_impl_opengl3.h"

#include "test/TestClearColor.h"
#include "test/TestRenderTexture.h"
#include "test/TestTwoBlock.h"
#include "test/TestDynamicDraw.h"

struct ImguiSettings
{
	glm::vec3 TransitaionA = glm::vec3(200, 100, 0);
	glm::vec3 TransitaionB = glm::vec3(400, 300, 0);
};

ImguiSettings Settings;

void doImguiFrame(ImGuiIO& io)
{
	static bool show_demo_window = true;
	static bool show_another_window = true;
	static ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	//if (show_demo_window)
	//	ImGui::ShowDemoWindow(&show_demo_window);

	// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
	{
		static float f = 0.0f;
		static int counter = 0;

		ImGui::Begin("Global Settings");                          // Create a window called "Hello, world!" and append into it.

		ImGui::SliderFloat3("Transition A", &Settings.TransitaionA.x, 0.0f, 960.0f);
		ImGui::SliderFloat3("Transition B", &Settings.TransitaionB.x, 0.0f, 960.0f);

		//ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
		//ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
		//ImGui::Checkbox("Another Window", &show_another_window);

		//ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
		//ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

		//if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
		//	counter++;
		//ImGui::SameLine();
		//ImGui::Text("counter = %d", counter);

		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		ImGui::End();
	}

	// 3. Show another simple window.
	//if (show_another_window)
	//{
	//	ImGui::Begin("Another Window", &show_another_window);   // Pass a pointer to our bool variable (the window will have a closing button that will clear the bool when clicked)
	//	ImGui::Text("Hello from another window!");
	//	if (ImGui::Button("Close Me"))
	//		show_another_window = false;
	//	ImGui::End();
	//}

}

void launch(GLFWwindow* window, ImGuiIO& io)
{
	// 设定宽高是 640, 480 比例是 4:3 
	//glm::mat4 proj = glm::ortho(-4.0f, 4.0f, -3.0f, 3.0f, -1.0f, 1.0f);

	glm::mat4 proj = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
	//glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(-100, 0, 0));	// 相机向左偏移 100
	glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0));
	glm::mat4 mvp = proj * view;

	// 不使用MVP矩阵计算时 
	//float positions[] = {
	//	-0.5f, -0.5f, 0.0f, 0.0f,
	//	 0.5f, -0.5f, 1.0f, 0.0f,
	//	 0.5f,  0.5f, 1.0f, 1.0f,
	//	-0.5f,  0.5f, 0.0f, 1.0f,
	//};		
	// 使用 MVP 矩阵计算
	//float positions[] = {
	//	100.0f, 100.0f, 0.0f, 0.0f,
	//	700.0f, 100.0f, 1.0f, 0.0f,
	//	700.0f, 400.0f, 1.0f, 1.0f,
	//	100.0f, 400.0f, 0.0f, 1.0f,
	//};
	// 使用 MVP 矩阵计算 并以 （0，0） 为中心，长宽为 100
	float positions[] = {
		-50, -50, 0.0f, 0.0f,
			50, -50, 1.0f, 0.0f,
			50,  50, 1.0f, 1.0f,
		-50,  50, 0.0f, 1.0f,
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

		doImguiFrame(io);

		//render.draw 里面已经绑定过了 不用再绑定
		//va.Bind();
		//ibo.Bind();

		r += increment;
		//shader.Bind();
		//shader.SetUniform4f("u_Color", r, .5f, .5f, 1.0f);

		if (r > 1.0f || r < 0.0f) {
			increment *= -1;
		}

		//GL_CALL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
		glm::mat4 translationA = glm::translate(glm::mat4(1.0f), Settings.TransitaionA);
		shader.SetUniformMat4f("u_MVP", mvp * translationA);
		render.Draw(va, ibo, shader);

		// 绘制第二张图片
		glm::mat4 translationB = glm::translate(glm::mat4(1.0f), Settings.TransitaionB);
		shader.SetUniformMat4f("u_MVP", mvp * translationB);
		render.Draw(va, ibo, shader);

		// Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
}

void launch2(GLFWwindow* window, ImGuiIO& io)
{
	//TestModule::TestClearColor TestApp;
	//TestModule::TestRenderTexture TestApp;
	//TestModule::TestTwoBlock TestApp;
	TestModule::TestDynamicDraw TestApp;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	TestApp.Init();

	while (!glfwWindowShouldClose(window))
	{
		// 设置 ImGUI 新一帧
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		TestApp.OnUpdate(0.017f);
		TestApp.OnRender();

		TestApp.OnImGuiRender();

		// 绘制 ImGUI
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	TestApp.Exit();
}

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

	// 初始化 Imgui
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// 设置 Imgui 样式
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
	ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
	const char* glsl_version = "#version 330 core";
	ImGui_ImplOpenGL3_Init(glsl_version);

	if (GLEW_OK != glewInit()) {
		std::cout << "Error: glewInit Faild" << std::endl;
	}

	//std::cout << glGetString(GL_VERSION) << std::endl;

	// 添加测试模块之前的讲解案例
	//launch(window, io);

	// 使用测试模块
	launch2(window, io);

	glfwTerminate();
	return 0;
}