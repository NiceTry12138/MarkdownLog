#include "TestCamera.h"
#include "../Util/RenderSettings.h"
#include "TestModuleManager.h"

TestCamera TestCamera::_self;

void TestCamera::OnEnter(GLFWwindow* window)
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

	//std::cout << glfwGetInputMode(window, GLFW_CURSOR) << std::endl;
	BindMouse(window);

}

void TestCamera::OnExit(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, nullptr);
}

void TestCamera::UpdateLogic(float delayTime)
{
	m_model = glm::mat4(1.0f);
	m_view = glm::lookAt(m_CameraLocation, m_CameraLocation + m_CameraForward, m_CameraUp);

	// 可能会更新窗口视口大小 每帧更新一下
	m_proj = glm::perspective(glm::radians(45.0f), (float)RSI->ViewportHeight / (float)RSI->ViewportWidth, 0.1f, 100.0f);
}

void TestCamera::ClearRender(GLFWwindow* window)
{
	//TestBase::ClearRender(window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TestCamera::Render(GLFWwindow* window)
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

void TestCamera::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("Camera");

	ImGui::SliderFloat("Camera Move Speed", &m_MoveSpeed, 0.0f, 0.1f);
	ImGui::SliderFloat("Camera Rotate Speed", &m_RorateSpeed, 0.01f, 0.1f);

	if (ImGui::Button("Close Window"))
		glfwSetWindowShouldClose(window, true);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::End();
}

void TestCamera::InputProcess(GLFWwindow* window)
{
	TestBase::InputProcess(window);


	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_CameraLocation -= glm::normalize(glm::cross(m_CameraForward, m_CameraUp)) * m_MoveSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_CameraLocation += glm::normalize(glm::cross(m_CameraForward, m_CameraUp)) * m_MoveSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_CameraLocation += m_CameraForward * m_MoveSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_CameraLocation -= m_CameraForward * m_MoveSpeed;
	}
	
	
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

void TestCamera::StaticMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	TestCamera* test = static_cast<TestCamera*>(glfwGetWindowUserPointer(window));
	if (test)
	{
		test->MouseCallback(window, xpos, ypos);
	}
}

void TestCamera::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (m_isFirstMouse)
	{
		m_LastMousePosX = xpos;
		m_LastMousePosY = ypos;
		m_isFirstMouse = false;
	}

	float offsetX = xpos - m_LastMousePosX;
	// OpenGL 屏幕空间 Y 轴正方向向下，所以向下移动时 ypos 会增加
	float offsetY = -(ypos - m_LastMousePosY);

	m_LastMousePosY = ypos;
	m_LastMousePosX = xpos;

	offsetX *= m_RorateSpeed;	// 比例变换
	offsetY *= m_RorateSpeed;

	m_CameraYaw += offsetX;
	m_CameraPitch += offsetY;

	// 限制 pitch 仰角的大小，你头向上仰也有一个限制 类似的
	if (m_CameraPitch > 89.0f || m_CameraPitch < -89.0f)
	{
		m_CameraPitch = glm::clamp(m_CameraPitch, -89.0f, 89.0f);
	}

	glm::vec3 front;
	front.x = cos(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
	front.y = sin(glm::radians(m_CameraPitch));
	front.z = sin(glm::radians(m_CameraYaw)) * cos(glm::radians(m_CameraPitch));
	m_CameraForward = glm::normalize(front);
}

void TestCamera::BindMouse(GLFWwindow* window)
{
	m_isFirstMouse = true;
	// 设置鼠标捕获和隐藏
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(window, this);
	glfwSetCursorPosCallback(window, &TestCamera::StaticMouseCallback);
}

void TestCamera::UnBindMouse(GLFWwindow* window)
{
	m_isFirstMouse = false;

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// 因为设置自己的鼠标绑定把 ImGUI 的鼠标绑定覆盖了 所以退出自己的鼠标绑定时 重新让 ImGUI 绑定
	ImGui_ImplGlfw_RestoreCallbacks(window);	
	ImGui_ImplGlfw_InstallCallbacks(window);
}
