#pragma once

#include "../vendor/glm/glm.hpp"
#include "../vendor/glm/gtc/matrix_transform.hpp"

#include "../vendor/imgui/imgui.h"
#include "../vendor/imgui/imgui_impl_glfw.h"
#include "../vendor/imgui/imgui_impl_opengl3.h"

#include "../Util.h"
#include "../VertexArray.h"
#include "../VertexBuffer.h"
#include "../VertexBufferLayout.h"
#include "../IndexBuffer.h"
#include "../Texture.h"
#include "../Shader.h"
#include "../Renderer.h"

namespace TestModule {
	class Test
	{
	public:
		Test() = default;
		virtual ~Test() = default;

		virtual void Init();
		virtual void Exit() {};

		virtual void OnUpdate(float deltaTIme) {};
		virtual void OnRender() {};
		virtual void OnImGuiRender() {};
	
	protected:
		glm::mat4 m_MVP;
	};
}

