#include "TestWithMouseBase.h"

void TestWithMouseBase::BindMouse(GLFWwindow* window)
{
	// 设置鼠标捕获和隐藏
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetWindowUserPointer(window, this);
	glfwSetCursorPosCallback(window, &TestWithMouseBase::StaticMouseCallback);
}

void TestWithMouseBase::UnBindMouse(GLFWwindow* window)
{
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

	// 因为设置自己的鼠标绑定把 ImGUI 的鼠标绑定覆盖了 所以退出自己的鼠标绑定时 重新让 ImGUI 绑定
	ImGui_ImplGlfw_RestoreCallbacks(window);
	ImGui_ImplGlfw_InstallCallbacks(window);
}

void TestWithMouseBase::StaticMouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	TestWithMouseBase* test = static_cast<TestWithMouseBase*>(glfwGetWindowUserPointer(window));
	if (test)
	{
		test->MouseCallback(window, xpos, ypos);
	}
}
