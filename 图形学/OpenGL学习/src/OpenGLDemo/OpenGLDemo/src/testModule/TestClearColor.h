#pragma once
#include "TestBase.h"

class TestClearColor : public TestBase
{
public:
	TestClearColor() : TestBase("TestClearColor") {}

public:
	virtual void OnEnter(GLFWwindow* window) override;
	virtual void OnExit(GLFWwindow* window) override;

	virtual void UpdateLogic(float delayTime) override;
	virtual void ClearRender(GLFWwindow* window) override;
	virtual void Render(GLFWwindow* window) override;
	virtual void UpdateImGUI(GLFWwindow* window) override;

private:
	float m_ClearColor[3] = { 0.0f, 0.0f ,0.0f };
	bool m_UseImGUI = true;

	bool m_IsAdd = true;

	static TestClearColor _self;
};

