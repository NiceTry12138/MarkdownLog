#include "TestFrameBuffer.h"
#include "../Util/RenderSettings.h"

TestFrameBuffer TestFrameBuffer::_self;

void TestFrameBuffer::OnEnter(GLFWwindow* window)
{
	// 启动深度测试
	glEnable(GL_DEPTH_TEST);

	// 初始化 shader
	m_Shader.Init("res/shader/model/model.vert", "res/shader/model/model.frag");
	m_Shader.UnBind();

	m_LightShader.Init("res/shader/Light/LightVertex.vert", "res/shader/Light/LightFragment.frag");
	m_LightShader.UnBind();

	//m_packageModel.Init("res/model/armor-set/armor 2021.obj");
	m_packageModel.Init("res/model/Miku/miku_prefab.fbx");

	m_Camera.SetLocation(glm::vec3(0.0f, 0.0f, 3.0f));
	BindMouse(window);

	m_planeShader.Init("res/shader/FrameBuffer/fb.vert", "res/shader/FrameBuffer/fb.frag");
	m_planeShader.SetUniform1i("screenTexture", 10);
	m_planeShader.SetUniform1f("u_BlurSize", 1.0f / RSI->ViewportWidth);
	m_planeShader.SetUniform1f("m_blurTimes", m_blurTimes);
	m_planeShader.UnBind();

	InitFBO();

	m_ModelRotate = glm::vec3(-90.0f, 0.0f, 0.0f);

	m_Light.Init();
	m_LightPos = glm::vec3(5.0f);

	m_blurSize = RSI->ViewportWidth;
}

void TestFrameBuffer::OnExit(GLFWwindow* window)
{
	// 删除帧缓冲
	glDeleteFramebuffers(1, &m_FBO);

	glfwSetWindowUserPointer(window, nullptr);
	UnBindMouse(window);
}

void TestFrameBuffer::UpdateLogic(float delayTime)
{
	m_view = m_Camera.GetView();
	m_Camera.SetMoveSpeed(m_CameraMoveSpeed);
	m_Camera.SetRotateSpeed(m_CameraRotateSpeed);

	m_Light.SetLocation(m_LightPos);

	// 可能会更新窗口视口大小 每帧更新一下
	m_proj = glm::perspective(glm::radians(45.0f), (float)RSI->ViewportHeight / (float)RSI->ViewportWidth, 0.1f, 100.0f);
}

void TestFrameBuffer::ClearRender(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TestFrameBuffer::Render(GLFWwindow* window)
{
	if (m_ShowFBO)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);
	}

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	ClearRender(window);
	RenderModel();

	if (m_ShowFBO)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glDisable(GL_DEPTH_TEST);
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		m_planeShader.Bind();
		glBindVertexArray(m_VAOPlane);
		glActiveTexture(GL_TEXTURE10);
		glBindTexture(GL_TEXTURE_2D, m_textureBuffer);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}
}

void TestFrameBuffer::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("Model");

	ImGui::Checkbox("Show FBO Plane?", &m_ShowFBO);
	ImGui::SliderInt("Blue Times ", &m_blurTimes, 1, 5);

	if (ImGui::Button("Close Window"))
		glfwSetWindowShouldClose(window, true);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::End();
}

void TestFrameBuffer::InputProcess(GLFWwindow* window)
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

void TestFrameBuffer::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	m_Camera.MouseCallback(window, xpos, ypos);
}

void TestFrameBuffer::BindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(true);
	TestWithMouseBase::BindMouse(window);
}

void TestFrameBuffer::UnBindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(false);
	TestWithMouseBase::UnBindMouse(window);
}

void TestFrameBuffer::InitFBO()
{
	std::vector<Vertex_v2> planeVerteices = {
		{-1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f},
		{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},

		{-1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
		{ 1.0f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f},
		{ 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f},
	};

	// 渲染用 Plane 的效果
	GLuint planeVBO;

	glGenVertexArrays(1, &m_VAOPlane);
	glBindVertexArray(m_VAOPlane);

	glGenBuffers(1, &planeVBO);
	glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
	glBufferData(GL_ARRAY_BUFFER, planeVerteices.size() * sizeof(Vertex_v2), planeVerteices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_v2), (void*)offsetof(Vertex_v2, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_v2), (void*)offsetof(Vertex_v2, normal));
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_v2), (void*)offsetof(Vertex_v2, texCoords));

	glBindVertexArray(GL_ZERO);

	// 创建 Frame Buffer Obejct
	glGenFramebuffers(1, &m_FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

	// 创建贴图 用于承接帧缓冲并传递给 Shader 绘制
	glGenTextures(1, &m_textureBuffer);
	glBindTexture(GL_TEXTURE_2D, m_textureBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, RSI->ViewportWidth, RSI->ViewportHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_textureBuffer, 0);
	
	// 添加深度缓冲
	GLuint rbo;
	glGenRenderbuffers(1, &rbo);
	glBindRenderbuffer(GL_RENDERBUFFER, rbo);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, RSI->ViewportWidth, RSI->ViewportHeight);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "Error: FrameBuffer Is Not Compelete" << std::endl;
	}

	// 重置设置帧缓冲为 0，默认绘制到屏幕上
	glBindFramebuffer(GL_FRAMEBUFFER, GL_ZERO);
	glBindTexture(GL_TEXTURE_2D, GL_ZERO);
}

void TestFrameBuffer::RenderModel()
{
	m_Shader.Bind();

	auto CameraLocation = m_Camera.GetCameraLocation();

	auto model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(m_ModelRotate.x), glm::vec3(1, 0, 0));
	model = glm::rotate(model, glm::radians(m_ModelRotate.y), glm::vec3(0, 1, 0));
	model = glm::rotate(model, glm::radians(m_ModelRotate.z), glm::vec3(0, 0, 1));
	model = glm::scale(model, glm::vec3(m_ModelScale, m_ModelScale, m_ModelScale));

	m_Shader.SetUniformMat4f("model", model);
	m_Shader.SetUniformMat4f("view", m_view);
	m_Shader.SetUniformMat4f("projection", m_proj);
	m_Shader.SetUniform3f("lightPos", m_LightPos.x, m_LightPos.y, m_LightPos.z);
	m_Shader.SetUniform3f("viewPos", CameraLocation.x, CameraLocation.y, CameraLocation.z);
	m_Shader.SetUniform1i("shineness", m_Shineness);

	m_packageModel.Draw(m_Shader);

	glBindVertexArray(m_LightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);

	auto lightModule = glm::mat4(1.0f);
	lightModule = glm::translate(lightModule, m_LightPos);
	lightModule = glm::scale(lightModule, glm::vec3(0.2f)); // a smaller cube

	m_LightShader.Bind();
	m_LightShader.SetUniformMat4f("model", lightModule);
	m_LightShader.SetUniformMat4f("view", m_view);
	m_LightShader.SetUniformMat4f("projection", m_proj);
	m_Light.Draw();
}
