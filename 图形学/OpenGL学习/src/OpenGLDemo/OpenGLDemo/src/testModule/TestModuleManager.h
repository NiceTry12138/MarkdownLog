#pragma once

#include "../Util/CommonHead.h"
#include "../Util/UtilTemplate.h"
#include "TestBase.h"

class TestModuleManager : public Instance<TestModuleManager>
{
public:
	void AddTestModule(const std::string& Title, TestBase* TestModule);
	void NoSelectTestModule(GLFWwindow* window);
	
	void OnEnter(GLFWwindow* window);
	void OnExit(GLFWwindow* window);
	void Update(GLFWwindow* window, float deltaTime);

protected:
	TestBase* FindTestModule(const std::string& Title);

	void RenderImGUI(GLFWwindow* window, float deltaTime);
	void SelectTestModule(const std::string& Title, GLFWwindow* window);

	void TestModuleRun(GLFWwindow* window, float deltaTime);

private:
	std::string m_CurrentTitle;
	std::map<std::string, TestBase*> m_TestModules;
};

