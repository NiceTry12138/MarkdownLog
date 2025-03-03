#pragma once
#include "TestWithMouseBase.h"
#include "../Util/CommonData.h"

#include "../Util/Shader.h"
#include "../Util/Texture.h"
#include "../Util/Camera.h"

class TestLight : public TestWithMouseBase
{
public:
	virtual void OnEnter(GLFWwindow* window) override;
	virtual void OnExit(GLFWwindow* window) override;

	virtual void UpdateLogic(float delayTime) override;
	virtual void ClearRender(GLFWwindow* window) override;
	virtual void Render(GLFWwindow* window) override;
	virtual void UpdateImGUI(GLFWwindow* window) override;

	virtual void InputProcess(GLFWwindow* window) override;

	virtual void MouseCallback(GLFWwindow* window, double xpos, double ypos) override;

	virtual void BindMouse(GLFWwindow* window) override;
	virtual void UnBindMouse(GLFWwindow* window) override;
public:


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

	float m_CameraMoveSpeed = 0.1f;
	float m_CameraRotateSpeed = 0.1f;

	Camera m_Camera;
	bool m_bLeftAltPress = false;
};

