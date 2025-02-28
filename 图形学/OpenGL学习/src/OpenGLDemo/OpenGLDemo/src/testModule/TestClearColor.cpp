#include "TestClearColor.h"

void TestClearColor::OnEnter(GLFWwindow* window)
{

}

void TestClearColor::OnExit(GLFWwindow* window)
{

}

void TestClearColor::UpdateLogic(float delayTime)
{

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
	ImGui::Begin("ClearColor");
	ImGui::SliderFloat3("ClearColor", m_ClearColor, 0.0f, 1.0f);
	if (ImGui::Button("Close Window"))
		glfwSetWindowShouldClose(window, true);
	ImGui::End();
}
