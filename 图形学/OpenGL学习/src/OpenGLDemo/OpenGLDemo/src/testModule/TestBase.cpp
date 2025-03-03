#include "TestBase.h"
#include "TestModuleManager.h"

TestBase::TestBase(const std::string& TestModuleName)
{
	m_TestModuleName = TestModuleName;
	TestModuleManager::GetInstance()->AddTestModule(m_TestModuleName, this);
}

void TestBase::OnEnter(GLFWwindow* window)
{
}

void TestBase::OnExit(GLFWwindow* window)
{
}

void TestBase::Update(GLFWwindow* window, float delayTime)
{

    UpdateLogic(delayTime);
}

void TestBase::ClearRender(GLFWwindow* window)
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void TestBase::Render(GLFWwindow* window)
{
}

void TestBase::RenderImGUI(GLFWwindow* window)
{
	// 设置 ImGUI 新一帧
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Common Test Title");
	if (ImGui::Button("Back Test"))
		TestModuleManager::GetInstance()->NoSelectTestModule(window);
	ImGui::End();

	UpdateImGUI(window);

	// 绘制 ImGUI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void TestBase::UpdateImGUI(GLFWwindow* window)
{

}

void TestBase::UpdateLogic(float delayTime)
{
}

void TestBase::InputProcess(GLFWwindow* window)
{
    // 监听 ECS 按键
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
