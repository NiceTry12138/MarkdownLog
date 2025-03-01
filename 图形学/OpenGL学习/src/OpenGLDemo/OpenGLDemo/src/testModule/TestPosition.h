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
	GLuint m_VBO{ GL_ZERO };
	GLuint m_VAO{ GL_ZERO };
	GLuint m_IB{ GL_ZERO };

	Shader m_Shader;
	Texture m_Tex1;
	Texture m_Tex2;

	std::vector<Vertex_v0> m_vertexs;

	glm::mat4 m_model = glm::mat4(1.0f);		// 模型矩阵
	glm::mat4 m_view = glm::mat4(1.0f);			// 视图矩阵
	glm::mat4 m_proj = glm::mat4(1.0f);			// 投影矩阵

	glm::vec3 m_Transition = glm::vec3(0.0f, 0.0f, -3.0f);

	float m_Rotate = -45.0f;
	float m_RotateSpeed = 1.0f;

	bool m_RotateX = true;
	bool m_RotateY = false;
	bool m_RotateZ = false;
};

