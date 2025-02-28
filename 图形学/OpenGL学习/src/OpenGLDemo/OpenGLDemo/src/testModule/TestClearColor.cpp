#include "TestClearColor.h"

void TestClearColor::OnEnter(GLFWwindow* window)
{

}

void TestClearColor::OnExit(GLFWwindow* window)
{

}

void TestClearColor::UpdateLogic(float delayTime)
{
	if (!m_UseImGUI)
	{
		float rate = m_IsAdd ? 1 : -1;
		m_ClearColor[0] += rate * delayTime;
		m_ClearColor[1] += rate * delayTime * 2;
		m_ClearColor[2] += rate * delayTime * 3;

		if (m_ClearColor[0] > 1.0f || m_ClearColor[0] < 0.0f) {
			m_ClearColor[0] = m_IsAdd ? 1.0f : 0.0f;
			m_ClearColor[1] = m_IsAdd ? 1.0f : 0.0f;
			m_ClearColor[2] = m_IsAdd ? 1.0f : 0.0f;
			m_IsAdd = !m_IsAdd;
		}
	}
}

void TestClearColor::ClearRender(GLFWwindow* window)
{
	glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
}

void TestClearColor::Render(GLFWwindow* window)
{
}

void TestClearColor::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("ClearColor");
	ImGui::Checkbox("ClearColorWithTime", &m_UseImGUI);    
	if (m_UseImGUI)
	{
		ImGui::SliderFloat3("ClearColor", m_ClearColor, 0.0f, 1.0f);
	}
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	if (ImGui::Button("Close Window"))
		glfwSetWindowShouldClose(window, true);
	ImGui::End();
}
