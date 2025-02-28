#pragma once

#include "../Util/CommonHead.h"

class TestBase
{
public:
	virtual ~TestBase() = default;

	// ������� ��ʼ���߼�
	virtual void OnEnter(GLFWwindow* window);
	// �˳����� �����߼�
	virtual void OnExit(GLFWwindow* window);

	void Update(GLFWwindow* window, float delayTime);
	
	// �Զ��� glClear �߼�
	virtual void ClearRender(GLFWwindow* window);

	// �Զ��� ��Ⱦ�߼�
	virtual void Render(GLFWwindow* window);

	// �Զ��� imGUI 
	virtual void UpdateImGUI(GLFWwindow* window);
	
	// �Զ��� �����߼�
	virtual void UpdateLogic(float delayTime);

	// ��������¼�
	virtual void InputProcess(GLFWwindow* window);
};

