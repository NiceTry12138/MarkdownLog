#pragma once
#include "TestWithMouseBase.h"
#include "../Util/CommonData.h"

#include "../Util/Shader.h"
#include "../Util/Texture.h"
#include "../Util/Camera.h"
#include "../Util/Model.h"

#include "../Util/BaseLight.h"

class TestModelV1 : public TestWithMouseBase
{
public:
	TestModelV1() : TestWithMouseBase("TestModelV1") {}

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

protected:

private:
	GLuint m_LightVAO{ GL_ZERO };

	Shader m_Shader;
	Shader m_LightShader;
	std::vector<Vertex_v2> m_vertexs;

	glm::mat4 m_model = glm::mat4(1.0f);		// 模型矩阵
	glm::mat4 m_view = glm::mat4(1.0f);			// 视图矩阵
	glm::mat4 m_proj = glm::mat4(1.0f);			// 投影矩阵

	//glm::vec3 m_modelRotate = glm::vec3(0.0f);

	float m_CameraMoveSpeed = 0.1f;
	float m_CameraRotateSpeed = 0.1f;

	Model m_packageModel;

	Camera m_Camera;
	bool m_bLeftAltPress = false;

	BaseLight m_Light;
	glm::vec3 m_LightPos;

	float m_ModelScale = 1.0f;
	glm::vec3 m_ModelRotate;

	int m_Shineness = 4;

	static TestModelV1 _self;
};

