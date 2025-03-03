#pragma once

#include "../Util/CommonHead.h"
#include "TestBase.h"

class TestWithMouseBase : public TestBase
{
public:
	virtual void BindMouse(GLFWwindow* window);
	virtual void UnBindMouse(GLFWwindow* window);

	static void StaticMouseCallback(GLFWwindow* window, double xpos, double ypos);

	// 绑定之后 触发的鼠标事件回调函数
	virtual void MouseCallback(GLFWwindow* window, double xpos, double ypos) = 0;
};

