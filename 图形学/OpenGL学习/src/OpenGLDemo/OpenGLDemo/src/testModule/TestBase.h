#pragma once

#include "GLFW/glfw3.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"

class TestBase
{
public:
	virtual ~TestBase() = default;

	// 进入测试 初始化逻辑
	virtual void OnEnter(GLFWwindow* window);
	// 退出测试 清理逻辑
	virtual void OnExit(GLFWwindow* window);

	void Update(GLFWwindow* window, float delayTime);
	
	// 自定义 glClear 逻辑
	virtual void ClearRender(GLFWwindow* window);

	// 自定义 渲染逻辑
	virtual void Render(GLFWwindow* window);

	// 自定义 imGUI 
	virtual void UpdateImGUI(GLFWwindow* window);
	
	// 自定义 计算逻辑
	virtual void UpdateLogic(float delayTime);

	// 输出监听事件
	virtual void InputProcess(GLFWwindow* window);
};

