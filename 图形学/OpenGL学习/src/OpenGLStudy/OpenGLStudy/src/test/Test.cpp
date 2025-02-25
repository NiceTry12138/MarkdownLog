#include "Test.h"

void TestModule::Test::Init()
{
	m_MVP = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);
}
