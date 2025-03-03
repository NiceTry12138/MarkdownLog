#include "TestLight.h"
#include "../Util/RenderSettings.h"

TestLight TestLight::_self;

void TestLight::OnEnter(GLFWwindow* window)
{
	//					 坐标					颜色						UV 坐标			贴图序号
	m_vertexs.push_back({ 0.5f,  0.5f, -0.5f,	1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f,		0 });
	m_vertexs.push_back({ 0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 0.0f,		0 });
	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f,	0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		1 });
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f,	0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,		1 });
	m_vertexs.push_back({ 0.5f,  0.5f,  0.5f,	1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f,		0 });
	m_vertexs.push_back({ 0.5f, -0.5f,  0.5f,	1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 0.0f,		0 });
	m_vertexs.push_back({ -0.5f, -0.5f,  0.5f,	0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		1 });
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f,	0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,		1 });

	GLuint indices[] = {
		0, 1, 2, 2, 3, 0,			// 组成下面的两个三角形
		4, 5, 6, 6, 7, 4,			
		1, 2, 6, 6, 5, 1,
		0, 1, 5, 5, 4, 0,
		2, 3, 7, 7, 6, 2,
		0, 3, 4, 4, 7, 3,
	};
	
	// 启动深度测试
	glEnable(GL_DEPTH_TEST);

	// 创建 VAO
	glGenVertexArrays(1, &m_VAO);

	// 绑定 VAO
	glBindVertexArray(m_VAO);

	// 创建 VBO IB
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IB);

	// 绑定 VBO 和 数据
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, m_vertexs.size() * sizeof(Vertex_v0), m_vertexs.data(), GL_STATIC_DRAW));

	// 绑定 IB 和 数据
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	// 设置 VAO 内存结构
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_v0), (void *)offsetof(Vertex_v0, position)));

	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_v0), (void*)offsetof(Vertex_v0, color)));

	GL_CALL(glEnableVertexAttribArray(2));
	GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_v0), (void*)offsetof(Vertex_v0, texCoord)));

	GL_CALL(glEnableVertexAttribArray(3));
	GL_CALL(glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, sizeof(Vertex_v0), (void*)offsetof(Vertex_v0, texIndex)));

	// 初始化 shader
	//m_Shader.NewInit("res/shader/TextPosition/Vertex.vert", "res/shader/TextPosition/Fragment.frag");
	m_Shader.Init("res/shader/TextPosition/Vertex.vert", "res/shader/TextPosition/Fragment.frag");

	// 初始化 texture
	m_Tex1.Init("res/textures/test2.png");
	m_Tex2.Init("res/textures/test3.png");

	m_Tex1.Bind(0);
	m_Tex2.Bind(1);

	m_Shader.UnBind();

	BindMouse(window);
}

void TestLight::OnExit(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, nullptr);
	UnBindMouse(window);
}

void TestLight::UpdateLogic(float delayTime)
{
	m_model = glm::mat4(1.0f);
	m_view = m_Camera.GetView();
	m_Camera.SetMoveSpeed(m_CameraMoveSpeed);
	m_Camera.SetRotateSpeed(m_CameraRotateSpeed);

	// 可能会更新窗口视口大小 每帧更新一下
	m_proj = glm::perspective(glm::radians(45.0f), (float)RSI->ViewportHeight / (float)RSI->ViewportWidth, 0.1f, 100.0f);
}

void TestLight::ClearRender(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TestLight::Render(GLFWwindow* window)
{
	m_Shader.Bind();
	m_Shader.SetUniform1i("u_Texture0", 0);
	m_Shader.SetUniform1i("u_Texture1", 1);

	m_Shader.SetUniformMat4f("model", m_model);
	m_Shader.SetUniformMat4f("view", m_view);
	m_Shader.SetUniformMat4f("projection", m_proj);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);
}

void TestLight::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("Camera");

	ImGui::SliderFloat("Camera Move Speed", &m_CameraMoveSpeed, 0.0f, 0.1f);
	ImGui::SliderFloat("Camera Rotate Speed", &m_CameraRotateSpeed, 0.01f, 0.1f);

	if (ImGui::Button("Close Window"))
		glfwSetWindowShouldClose(window, true);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::End();
}

void TestLight::InputProcess(GLFWwindow* window)
{
	TestWithMouseBase::InputProcess(window);
	
	m_Camera.InputProcess(window);
	
	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		m_bLeftAltPress = true;
		UnBindMouse(window);
	}
	else if(m_bLeftAltPress) {
		// 因为当前状况 如果不按下 左 alt，每帧都会导致该函数触发，加个判断防止重复触发
		m_bLeftAltPress = false;
		BindMouse(window);
	}
}

void TestLight::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	m_Camera.MouseCallback(window, xpos, ypos);
}

void TestLight::BindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(true);
	TestWithMouseBase::BindMouse(window);
}

void TestLight::UnBindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(false);
	TestWithMouseBase::UnBindMouse(window);
}
