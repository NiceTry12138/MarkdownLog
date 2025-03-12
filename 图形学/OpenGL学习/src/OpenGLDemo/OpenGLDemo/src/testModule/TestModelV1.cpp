#include "TestModelV1.h"
#include "../Util/RenderSettings.h"

TestModelV1 TestModelV1::_self;

void TestModelV1::OnEnter(GLFWwindow* window)
{
	// 启动深度测试
	glEnable(GL_DEPTH_TEST);

	// 初始化 shader
	m_Shader.Init("res/shader/model/model.vert", "res/shader/model/model.frag");
	m_Shader.UnBind();

	m_LightShader.Init("res/shader/Light/LightVertex.vert", "res/shader/Light/LightFragment.frag");
	m_LightShader.UnBind();

	m_packageModel.Init("res/model/armor-set/armor 2021.obj");
	//m_packageModel.Init("res/model/Miku/miku_prefab.fbx");

	m_Camera.SetLocation(glm::vec3(0.0f, 0.0f, 3.0f));
	BindMouse(window);

	m_Light.Init();
	m_LightPos = glm::vec3(5.0f);
}

void TestModelV1::OnExit(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, nullptr);
	UnBindMouse(window);
}

void TestModelV1::UpdateLogic(float delayTime)
{
	m_view = m_Camera.GetView();
	m_Camera.SetMoveSpeed(m_CameraMoveSpeed);
	m_Camera.SetRotateSpeed(m_CameraRotateSpeed);

	m_Light.SetLocation(m_LightPos);

	// 可能会更新窗口视口大小 每帧更新一下
	m_proj = glm::perspective(glm::radians(45.0f), (float)RSI->ViewportHeight / (float)RSI->ViewportWidth, 0.1f, 100.0f);
}

void TestModelV1::ClearRender(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TestModelV1::Render(GLFWwindow* window)
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

void TestModelV1::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("Model");

	ImGui::SliderFloat3("Light Position", &m_LightPos.x, -10.0f, 10.0f);
	ImGui::SliderFloat("Model Scale", &m_ModelScale, 0.1f, 2.0f);
	ImGui::SliderFloat3("Model Rotate", &m_ModelRotate.x, -180.0f, 180.0f);
	ImGui::SliderInt("Model Shineness", &m_Shineness, 0, 64);

	if (ImGui::Button("Close Window"))
		glfwSetWindowShouldClose(window, true);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	ImGui::End();
}

void TestModelV1::InputProcess(GLFWwindow* window)
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

void TestModelV1::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	m_Camera.MouseCallback(window, xpos, ypos);
}

void TestModelV1::BindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(true);
	TestWithMouseBase::BindMouse(window);
}

void TestModelV1::UnBindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(false);
	TestWithMouseBase::UnBindMouse(window);
}
