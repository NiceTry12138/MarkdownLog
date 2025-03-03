#pragma once
#include "TestBase.h"
#include "../Util/CommonData.h"

#include "../Util/Shader.h"
#include "../Util/Texture.h"

class TestCamera : public TestBase
{
public:
	TestCamera() : TestBase("TestCamera") {}

public:
	virtual void OnEnter(GLFWwindow* window) override;
	virtual void OnExit(GLFWwindow* window) override;

	virtual void UpdateLogic(float delayTime) override;
	virtual void ClearRender(GLFWwindow* window) override;
	virtual void Render(GLFWwindow* window) override;
	virtual void UpdateImGUI(GLFWwindow* window) override;

	virtual void InputProcess(GLFWwindow* window) override;

public:
	static void StaticMouseCallback(GLFWwindow* window, double xpos, double ypos);

	void MouseCallback(GLFWwindow* window, double xpos, double ypos);

protected:
	void BindMouse(GLFWwindow* window);
	void UnBindMouse(GLFWwindow* window);

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

	glm::vec3 m_CameraLocation = glm::vec3(0.0f, 0.0f, 3.0f);


	glm::vec3 m_CameraForward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float m_MoveSpeed = 0.1f;
	float m_RorateSpeed = 0.1f;

	float m_CameraPitch = 0.0f;
	float m_CameraYaw = -90.0f;

	float m_LastMousePosX;
	float m_LastMousePosY;
	bool m_isFirstMouse = true;			// 防止鼠标进入捕获状态时 突然闪烁

	bool m_bLeftAltPress = false;

private:
	static TestCamera _self;			// 静态对象 编译时构造 自动注册
};

