#include "TestBase.h"

void TestBase::OnEnter(GLFWwindow* window)
{
}

void TestBase::OnExit(GLFWwindow* window)
{
}

void TestBase::Update(GLFWwindow* window, float delayTime)
{
    // 设置 ImGUI 新一帧
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    UpdateImGUI(window);

    // 绘制 ImGUI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    UpdateLogic(delayTime);
}

void TestBase::ClearRender(GLFWwindow* window)
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void TestBase::Render(GLFWwindow* window)
{
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
