#pragma once
#include "TestBase.h"
#include "../Util/CommonData.h"

#include "../Util/Shader.h"
#include "../Util/Texture.h"

class TestPosition : public TestBase
{
public:
	virtual void OnEnter(GLFWwindow* window) override;
	virtual void OnExit(GLFWwindow* window) override;

	virtual void UpdateLogic(float delayTime) override;
	virtual void ClearRender(GLFWwindow* window) override;
	virtual void Render(GLFWwindow* window) override;
	virtual void UpdateImGUI(GLFWwindow* window) override;

private:
	GLuint m_VBO;
	GLuint m_VAO;
	GLuint m_IB;

	Shader m_Shader;
	Texture m_Tex1;
	Texture m_Tex2;

	std::vector<Vertex_v0> m_vertexs;
};

