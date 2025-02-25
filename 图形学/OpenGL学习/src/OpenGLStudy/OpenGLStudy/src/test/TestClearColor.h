#pragma once
#include "Test.h"

namespace TestModule {
    class TestClearColor : public Test
    {
	public:
		virtual void OnUpdate(float deltaTIme);
		virtual void OnRender();
		virtual void OnImGuiRender();

	private:
		float m_ClearColor[4] = { 0.2f, 0.3f, 0.8f, 1.0f };
    };
};