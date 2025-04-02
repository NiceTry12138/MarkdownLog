#include "TestGLInstance.h"
#include "../Util/RenderSettings.h"
#include "TestModuleManager.h"
#include <random>

TestGLInstance TestGLInstance::_self;

static GLuint UBOSLOT = 2;

void TestGLInstance::OnEnter(GLFWwindow* window)
{	// 启动深度测试
	glEnable(GL_DEPTH_TEST);

	// 初始化 shader
	m_ModelShader.Init("res/shader/Instanced/instanced.vert", "res/shader/Instanced/instanced.frag");
	m_packageModel.Init("res/model/Miku/miku_prefab.fbx");

	m_Camera.SetLocation(glm::vec3(0.0f, 0.0f, 3.0f));
	BindMouse(window);

	InitSkyBox();
	InitUBO();
	InitGVBO();

	glBindBuffer(GL_ARRAY_BUFFER, m_GVBO);
	m_packageModel.AddInstanceData();
}

void TestGLInstance::OnExit(GLFWwindow* window)
{
	glfwSetWindowUserPointer(window, nullptr);
	UnBindMouse(window);
}

void TestGLInstance::UpdateLogic(float delayTime)
{
	m_view = m_Camera.GetView();
	m_Camera.SetMoveSpeed(0.2f);
	m_Camera.SetRotateSpeed(0.2f);

	// 可能会更新窗口视口大小 每帧更新一下
	m_proj = glm::perspective(glm::radians(45.0f), (float)RSI->ViewportHeight / (float)RSI->ViewportWidth, 0.1f, 100.0f);

	UpdateUBO();
	UpdateGVBOData(delayTime);
}

void TestGLInstance::ClearRender(GLFWwindow* window)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TestGLInstance::Render(GLFWwindow* window)
{
	glBindBufferBase(GL_UNIFORM_BUFFER, UBOSLOT, m_UBO);

	m_ModelShader.Bind();

	auto CameraLocation = m_Camera.GetCameraLocation();
	m_ModelShader.BindUBO("Matrices", UBOSLOT);

	m_ModelShader.SetUniform1i("UseInstanceMatrix", m_useInstance ? 10 : 0);
	if (!m_useInstance)
	{
		for (const auto& model : m_InstanceData)
		{
			m_ModelShader.SetUniformMat4f("model", model);
			m_packageModel.Draw(m_ModelShader);
		}
	}
	else {
		m_packageModel.Draw(m_ModelShader, m_InstanceData.size());
	}
	
	m_sky.Draw(m_view, m_proj);
}

void TestGLInstance::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("Instance");

	ImGui::Checkbox("Use Instance", &m_useInstance);
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);

	ImGui::End();
}

void TestGLInstance::InputProcess(GLFWwindow* window)
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

void TestGLInstance::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	m_Camera.MouseCallback(window, xpos, ypos);
}

void TestGLInstance::BindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(true);
	TestWithMouseBase::BindMouse(window);
}

void TestGLInstance::UnBindMouse(GLFWwindow* window)
{
	m_Camera.SetFirstMouse(false);
	TestWithMouseBase::UnBindMouse(window);
}

void TestGLInstance::InitSkyBox()
{
	m_sky.Init();
}

void TestGLInstance::InitUBO()
{
	GL_CALL(glGenBuffers(1, &m_UBO));
	GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_UBO));
	GL_CALL(glBufferData(GL_UNIFORM_BUFFER, sizeof(glm::mat4) * 2, NULL, GL_DYNAMIC_DRAW));
}

void TestGLInstance::UpdateUBO()
{
	GL_CALL(glBindBuffer(GL_UNIFORM_BUFFER, m_UBO));
	GLvoid* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

	memcpy(ptr, &m_view, sizeof(m_view));
	memcpy((char*)ptr + sizeof(m_view), &m_proj, sizeof(m_proj));

	glUnmapBuffer(GL_UNIFORM_BUFFER);
}

void TestGLInstance::InitGVBO()
{
	const float Radius = 50.0f;			// 旋转半径

	srand(glfwGetTime()); // 初始化随机种子    

	// 使用更精确的随机数生成
	std::mt19937 rng(std::random_device{}());
	std::uniform_real_distribution<float> dist_z(-5.0f, 5.0f);      // Z轴范围扩大
	std::uniform_real_distribution<float> dist_scale(0.5f, 1.5f);      // 缩放范围
	std::uniform_real_distribution<float> dist_angle(0.0f, 2.0f * glm::pi<float>()); // 弧度制随机角度
	std::uniform_real_distribution<float> dist_axis(-1.0f, 1.0f);      // 随机旋转轴

	for (int index = 0; index < m_InstanceNum; ++index)
	{
		glm::mat4 model = glm::mat4(1.0f); // 显式初始化单位矩阵

		// 随机圆周分布（带随机半径偏移）
		float angle = dist_angle(rng);
		float radius_variation = 0.8f + 0.2f * (rng() / (float)rng.max()); // 半径80%~100%
		float x = cos(angle) * Radius * radius_variation;
		float z = sin(angle) * Radius * radius_variation;
		float y = dist_z(rng); // 使用连续随机分布

		model = glm::translate(model, glm::vec3(x, y, z));

		// 随机缩放
		float scale = dist_scale(rng);
		model = glm::scale(model, glm::vec3(scale));

		// 随机旋转轴和角度
		glm::vec3 rot_axis = glm::normalize(glm::vec3(
			dist_axis(rng),
			dist_axis(rng),
			dist_axis(rng)
		));

		float rot_angle = dist_angle(rng); // 直接使用弧度制
		model = glm::rotate(model, rot_angle, rot_axis);

		m_InstanceData.push_back(model);
	}

	glBindVertexArray(GL_ZERO);

	glGenBuffers(1, &m_GVBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_GVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * m_InstanceData.size(), m_InstanceData.data(), GL_DYNAMIC_DRAW);
}

void TestGLInstance::UpdateGVBOData(float delayTime)
{
	for (int index = 0; index < m_InstanceNum; ++index)
	{
	}
}
