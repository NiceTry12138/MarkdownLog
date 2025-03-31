#include "TestGeoShader.h"
#include "../Util/RenderSettings.h"
#include "TestModuleManager.h"

TestGeoShader TestGeoShader::_self;

static GLuint UBOSLOT = 2;

void TestGeoShader::OnEnter(GLFWwindow* window)
{	// 启动深度测试
	glEnable(GL_DEPTH_TEST);

	// 初始化 shader
	m_ModelShader.Init("res/shader/GeoShader/model.vert", "res/shader/GeoShader/model.frag");
	m_packageModel.Init("res/model/Miku/miku_prefab.fbx");

	m_GeoShader.Init("res/shader/GeoShader/line.vs", "res/shader/GeoShader/line.fs", "res/shader/GeoShader/line.gs");

	m_Camera.SetLocation(glm::vec3(0.0f, 0.0f, 3.0f));
	BindMouse(window);

	InitSkyBox();
	InitUBO();
}

void TestGeoShader::OnExit(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, nullptr);
	UnBindMouse(window);
}

void TestGeoShader::UpdateLogic(float delayTime)
{
	m_view = m_Camera.GetView();
	m_Camera.SetMoveSpeed(0.2f);
	m_Camera.SetRotateSpeed(0.2f);

	// 可能会更新窗口视口大小 每帧更新一下
	m_proj = glm::perspective(glm::radians(45.0f), (float)RSI->ViewportHeight / (float)RSI->ViewportWidth, 0.1f, 100.0f);

	UpdateUBO();
}

void TestGeoShader::ClearRender(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TestGeoShader::Render(GLFWwindow* window)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, UBOSLOT, m_UBO);


	auto CameraLocation = m_Camera.GetCameraLocation();

	auto model = glm::mat4(1.0f);
	model = glm::rotate(model, glm::radians(-90.0f), glm::vec3(1, 0, 0));

	m_ModelShader.Bind();
	m_ModelShader.SetUniformMat4f("model", model);
	m_ModelShader.BindUBO("Matrices", UBOSLOT);
	m_packageModel.Draw(m_ModelShader);

	m_GeoShader.Bind();
	m_GeoShader.SetUniformMat4f("model", model);
	m_GeoShader.BindUBO("Matrices", UBOSLOT);
	m_packageModel.Draw(m_GeoShader);

	m_sky.Draw(m_view, m_proj);
}

void TestGeoShader::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("SkyBox");

	ImGui::End();
}

void TestGeoShader::InputProcess(GLFWwindow* window)
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

void TestGeoShader::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	m_Camera.MouseCallback(window, xpos, ypos);
}

void TestGeoShader::BindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(true);
	TestWithMouseBase::BindMouse(window);
}

void TestGeoShader::UnBindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(false);
	TestWithMouseBase::UnBindMouse(window);
}

void TestGeoShader::InitSkyBox()
{
	m_sky.Init();
}

void TestGeoShader::InitUBO()
{
	GL_CALL(glGenBuffers(1, &m_UBO));
	GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_UBO));
	GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_DYNAMIC_DRAW));
}

void TestGeoShader::UpdateUBO()
{
	GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_UBO));
	GLvoid* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

	memcpy(ptr, &m_view, sizeof(m_view));
	memcpy((char*)ptr + sizeof(m_view), &m_proj, sizeof(m_proj));

	glUnmapBuffer(GL_UNIFORM_BUFFER);
}
