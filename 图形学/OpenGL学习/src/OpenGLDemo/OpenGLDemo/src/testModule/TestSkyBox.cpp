#include "TestSkyBox.h"
#include "../Util/RenderSettings.h"
#include "TestModuleManager.h"

TestSkyBox TestSkyBox::_self;

void TestSkyBox::OnEnter(GLFWwindow* window)
{	// 启动深度测试
	glEnable(GL_DEPTH_TEST);

	// 初始化 shader
	m_ModelShader.Init("res/shader/model/model.vert", "res/shader/model/model.frag");
	m_packageModel.Init("res/model/Miku/miku_prefab.fbx");

	m_Camera.SetLocation(glm::vec3(0.0f, 0.0f, 3.0f));
	BindMouse(window);

	InitSkyBox();
}

void TestSkyBox::OnExit(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, nullptr);
	UnBindMouse(window);
}

void TestSkyBox::UpdateLogic(float delayTime)
{
	m_view = m_Camera.GetView();
	m_Camera.SetMoveSpeed(1);
	m_Camera.SetRotateSpeed(1);

	// 可能会更新窗口视口大小 每帧更新一下
	m_proj = glm::perspective(glm::radians(45.0f), (float)RSI->ViewportHeight / (float)RSI->ViewportWidth, 0.1f, 100.0f);
}

void TestSkyBox::ClearRender(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TestSkyBox::Render(GLFWwindow* window)
{
	m_ModelShader.Bind();


	auto CameraLocation = m_Camera.GetCameraLocation();

	m_ModelShader.SetUniformMat4f("model", glm::mat4(1.0f));
	m_ModelShader.SetUniformMat4f("view", m_view);
	m_ModelShader.SetUniformMat4f("projection", m_proj);
	m_ModelShader.SetUniform3f("lightPos", 0.0f, 0.0f, 0.0f);
	m_ModelShader.SetUniform3f("viewPos", CameraLocation.x, CameraLocation.y, CameraLocation.z);
	m_ModelShader.SetUniform1i("shineness", 4);

	m_packageModel.Draw(m_ModelShader);

	m_sky.Draw(m_view, m_proj);
}

void TestSkyBox::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("SkyBox");

	ImGui::End();
}

void TestSkyBox::InputProcess(GLFWwindow* window)
{
	TestWithMouseBase::InputProcess(window);

	m_Camera.InputProcess(window);

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS)
	{
		m_bLeftAltPress = true;
		UnBindMouse(window);
	}
	else if (m_bLeftAltPress) {
		// 因为当前状况 如果不按下 左 alt，每帧都会导致该函数触发，加个判断防止重复触发
		m_bLeftAltPress = false;
		BindMouse(window);
	}
}

void TestSkyBox::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	m_Camera.MouseCallback(window, xpos, ypos);
}

void TestSkyBox::BindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(true);
	TestWithMouseBase::BindMouse(window);
}

void TestSkyBox::UnBindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(false);
	TestWithMouseBase::UnBindMouse(window);
}

void TestSkyBox::InitSkyBox()
{
	m_sky.Init();
}
