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

	m_packageModel.Init("res/model/Miku/miku_prefab.fbx");

	m_Camera.SetLocation(glm::vec3(0.0f, 0.0f, 3.0f));
	BindMouse(window);

	m_model = glm::scale(m_model, glm::vec3(3.0f, 3.0f, 3.0));
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

	m_Shader.SetUniformMat4f("model", m_model);
	m_Shader.SetUniformMat4f("view", m_view);
	m_Shader.SetUniformMat4f("projection", m_proj);
	m_Shader.SetUniform3f("lightPos", CameraLocation.x, CameraLocation.y, CameraLocation.z);

	m_packageModel.Draw(m_Shader);

	glBindVertexArray(m_LightVAO);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
}

void TestModelV1::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("Model");

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
