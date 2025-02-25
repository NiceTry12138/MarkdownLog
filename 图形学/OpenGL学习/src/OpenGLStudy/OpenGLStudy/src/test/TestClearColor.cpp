#include "TestClearColor.h"
#include "../Util.h"

void TestModule::TestClearColor::OnUpdate(float deltaTIme)
{

}

void TestModule::TestClearColor::OnRender()
{
	GL_CALL(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
	GL_CALL(glClear(GL_COLOR_BUFFER_BIT));
}

void TestModule::TestClearColor::OnImGuiRender()
{
	ImGui::Begin("ClearColor");
	ImGui::SliderFloat3("Transition A", m_ClearColor, 0.0f, 1.0f);
	ImGui::End();
}
