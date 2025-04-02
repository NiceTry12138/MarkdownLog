#pragma once
#include "TestWithMouseBase.h"
#include "../Util/CommonData.h"

#include "../Util/Shader.h"
#include "../Util/Texture.h"
#include "../Util/Camera.h"
#include "../Util/Model.h"

#include "../Util/BaseLight.h"

#include "../Util/SkyBox.h"

class TestGLInstance : public TestWithMouseBase
{
public:
	TestGLInstance() : TestWithMouseBase("TestGLInstance") {}

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
	void InitSkyBox();
	
	void InitUBO();
	void UpdateUBO();

	void InitGVBO();
	void UpdateGVBOData(float delayTime);

private:
	glm::mat4 m_model = glm::mat4(1.0f);		// 模型矩阵
	glm::mat4 m_view = glm::mat4(1.0f);			// 视图矩阵
	glm::mat4 m_proj = glm::mat4(1.0f);			// 投影矩阵

	Camera m_Camera;
	bool m_bLeftAltPress = false;

	SkyBox m_sky;

	GLuint m_UBO;								// Uniform Buffer Obejct

	Shader m_ModelShader;
	Model m_packageModel;

	GLuint m_GVBO;								// 一个全局的 VBO 用于存储 实例化数组

	std::vector<glm::mat4> m_InstanceData;		

	int m_InstanceNum = 1000;

	bool m_useInstance = true;
	static TestGLInstance _self;
};

