#include "TestLight.h"
#include "../Util/RenderSettings.h"

TestLight TestLight::_self;

void TestLight::OnEnter(GLFWwindow* window)
{
	//					 坐标					顶点法线	
	m_vertexs.push_back({  0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f });
	m_vertexs.push_back({  0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f });
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f });
	m_vertexs.push_back({  0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f });
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f });
	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f });

	m_vertexs.push_back({  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({  0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({  0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({  0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({  0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f });

	m_vertexs.push_back({  0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, });
	m_vertexs.push_back({  0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, });
	m_vertexs.push_back({  0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, });

	m_vertexs.push_back({  0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, });
	m_vertexs.push_back({  0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, });
	m_vertexs.push_back({  0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, });

	m_vertexs.push_back({  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f });
	m_vertexs.push_back({  0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f });
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f });
	m_vertexs.push_back({  0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f });
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f });
	m_vertexs.push_back({ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f });

	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f });

	// 启动深度测试
	glEnable(GL_DEPTH_TEST);

	// 创建 VAO
	glGenVertexArrays(1, &m_VAO);

	// 绑定 VAO
	glBindVertexArray(m_VAO);

	// 创建 VBO IB
	glGenBuffers(1, &m_VBO);

	// 绑定 VBO 和 数据
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, m_vertexs.size() * sizeof(Vertex_v1), m_vertexs.data(), GL_STATIC_DRAW));

	// 偷懒 就不用 index buffer 了
	//GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB));
	//GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	// 设置 VAO 内存结构
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_v1), (void *)offsetof(Vertex_v1, position)));

	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_v1), (void*)offsetof(Vertex_v1, normal)));

	// 初始化 shader
	//m_Shader.NewInit("res/shader/TextPosition/Vertex.vert", "res/shader/TextPosition/Fragment.frag");
	m_Shader.Init("res/shader/Light/Vertex.vert", "res/shader/Light/Fragment.frag");

	m_Shader.UnBind();

	m_LightShader.Init("res/shader/Light/LightVertex.vert", "res/shader/Light/LightFragment.frag");
	m_LightShader.UnBind();

	InitLight();

	m_Camera.SetLocation(glm::vec3(0.0f, 0.0f, 3.0f));

	BindMouse(window);
}

void TestLight::OnExit(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, nullptr);
	UnBindMouse(window);
}

void TestLight::UpdateLogic(float delayTime)
{
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

	m_Shader.SetUniformMat4f("model", m_model);
	m_Shader.SetUniformMat4f("view", m_view);
	m_Shader.SetUniformMat4f("projection", m_proj);

	m_Shader.SetUniform3f("light.ambient", m_light.ambient.x, m_light.ambient.y, m_light.ambient.z);		// 设置强度
	m_Shader.SetUniform3f("light.diffuse", m_light.diffuse.x, m_light.diffuse.y, m_light.diffuse.z);
	m_Shader.SetUniform3f("light.specular", m_light.specular.x, m_light.specular.y, m_light.specular.z);
	m_Shader.SetUniform3f("light.lightPos", m_light.lightPos.x, m_light.lightPos.y, m_light.lightPos.z);

	m_Shader.SetUniform3f("cubeMaterial.ambient", m_cubeMaterial.ambient.x, m_cubeMaterial.ambient.y, m_cubeMaterial.ambient.z);
	m_Shader.SetUniform3f("cubeMaterial.diffuse", m_cubeMaterial.diffuse.x, m_cubeMaterial.diffuse.y, m_cubeMaterial.diffuse.z);
	m_Shader.SetUniform3f("cubeMaterial.specular", m_cubeMaterial.specular.x, m_cubeMaterial.specular.y, m_cubeMaterial.specular.z );
	m_Shader.SetUniform1i("cubeMaterial.shininess", m_cubeMaterial.shininesss);

	m_Shader.SetUniform3f("viewPos", m_Camera.GetCameraLocation().x, m_Camera.GetCameraLocation().y, m_Camera.GetCameraLocation().z);

	glBindVertexArray(m_VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);

	auto lightModule = glm::mat4(1.0f);
	lightModule = glm::translate(lightModule, m_light.lightPos);
	lightModule = glm::scale(lightModule, glm::vec3(0.2f)); // a smaller cube

	m_LightShader.Bind();
	m_LightShader.SetUniformMat4f("model", lightModule);
	m_LightShader.SetUniformMat4f("view", m_view);
	m_LightShader.SetUniformMat4f("projection", m_proj);

	glBindVertexArray(m_LightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
}

void TestLight::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("Light");

	ImGui::SliderFloat3("Light ambient", &m_light.ambient.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("Light diffuse", &m_light.diffuse.x, 0.0f, 5.0f);
	ImGui::SliderFloat3("Light specular", &m_light.specular.x, 0.0f, 5.0f);
	ImGui::SliderFloat3("Light position", &m_light.lightPos.x, -5.0f, 5.0f);

	ImGui::SliderFloat3("Cube Material ambient", &m_cubeMaterial.ambient.x, 0.0f, 1.0f);
	ImGui::SliderFloat3("Cube Material diffuse", &m_cubeMaterial.diffuse.x, 0.0f, 5.0f);
	ImGui::SliderFloat3("Cube Material specular", &m_cubeMaterial.specular.x, 0.0f, 5.0f);
	ImGui::SliderInt("Cube Material position", &m_cubeMaterial.shininesss, 1, 100);


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

void TestLight::CreateLight()
{
	glGenVertexArrays(1, &m_LightVAO);
	glBindVertexArray(m_LightVAO);

	// 直接沿用立方体数据
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);	
	
	// 设置 VAO 内存结构 
	// 灯光只需要位置数据
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_v1), (void*)offsetof(Vertex_v1, position)));
}

void TestLight::InitLight()
{
	CreateLight();

	m_light.lightPos = glm::vec3(1.2f, 1.0f, 2.0f);
	m_light.ambient = glm::vec3(1.0f) * 0.1f;
	m_light.diffuse = glm::vec3(1.0f) * 1.0f;
	m_light.specular = glm::vec3(1.0f) * 0.5f;

	m_cubeMaterial.ambient = glm::vec3(1.0f, 0.5f, 0.31f);
	m_cubeMaterial.diffuse = glm::vec3(1.0f, 0.5f, 0.31f);
	m_cubeMaterial.specular = glm::vec3(0.5f, 0.5f, 0.5f);
	m_cubeMaterial.shininesss = 32;
}
