#include "TestModuleManager.h"

void TestModuleManager::AddTestModule(const std::string& Title, TestBase* TestModule)
{
	if (m_TestModules.find(Title) != m_TestModules.end())
	{
		std::cout << "Error: Had Added TestModule. Title = " << Title << std::endl;
		return;
	}

	m_TestModules[Title] = TestModule;
}

void TestModuleManager::NoSelectTestModule(GLFWwindow* window)
{
	auto TestModule = FindTestModule(m_CurrentTitle);
	if (TestModule)
	{
		TestModule->OnExit(window);
	}
	m_CurrentTitle.clear();
}

void TestModuleManager::OnEnter(GLFWwindow* window)
{
}

void TestModuleManager::OnExit(GLFWwindow* window)
{
	auto TestModule = FindTestModule(m_CurrentTitle);
	if (TestModule == nullptr)
	{
		return;
	}
	TestModule->OnExit(window);
}

void TestModuleManager::Update(GLFWwindow* window, float deltaTime)
{
	if (m_CurrentTitle.empty()) {
		RenderImGUI(window, deltaTime);
	}
	else {
		TestModuleRun(window, deltaTime);
	}
}

TestBase* TestModuleManager::FindTestModule(const std::string& Title)
{
	if (m_TestModules.find(Title) != m_TestModules.end())
	{
		return m_TestModules[Title];
	}

	return nullptr;
}

void TestModuleManager::RenderImGUI(GLFWwindow* window, float deltaTime)
{
	glClear(GL_COLOR_BUFFER_BIT);

	// 设置 ImGUI 新一帧
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::Begin("Common Test Title");
	for (const auto& Item : m_TestModules)
	{
		if (ImGui::Button(Item.first.c_str()))
		{
			SelectTestModule(Item.first, window);
		}
	}
	ImGui::End();

	// 绘制 ImGUI
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void TestModuleManager::SelectTestModule(const std::string& Title, GLFWwindow* window)
{
	m_CurrentTitle = Title;
	auto TestModule = FindTestModule(Title);
	if (TestModule == nullptr)
	{
		return;
	}

	TestModule->OnEnter(window);
}

void TestModuleManager::TestModuleRun(GLFWwindow* window, float deltaTime)
{
	auto TestModule = FindTestModule(m_CurrentTitle);
	if (TestModule == nullptr)
	{
		return;
	}

	TestModule->InputProcess(window);
	TestModule->Update(window, deltaTime);

	TestModule->ClearRender(window);
	TestModule->RenderImGUI(window);
	TestModule->Render(window);
}
