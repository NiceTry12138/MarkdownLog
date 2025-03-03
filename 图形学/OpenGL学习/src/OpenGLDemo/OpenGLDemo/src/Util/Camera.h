#pragma once
#include "CommonHead.h"

class Camera
{
public:
	void InputProcess(GLFWwindow* window);
	void MouseCallback(GLFWwindow* window, double xpos, double ypos);
	glm::mat4 GetView();

public:
	void SetFirstMouse(bool bIsFirst);
	void SetMoveSpeed(float moveSpeed);
	void SetRotateSpeed(float rotateSpeed);

private:
	glm::vec3 m_CameraLocation = glm::vec3(0.0f, 0.0f, 3.0f);
	glm::vec3 m_CameraForward = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 m_CameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float m_Pitch = 0;
	float m_Yaw = -90;
	float m_Roll = 0;

	float m_MoveSpeed = 0.1f;
	float m_RorateSpeed = 0.1f;

	float m_LastMousePosX;
	float m_LastMousePosY;
	bool m_isFirstMouse = true;			// 防止鼠标进入捕获状态时 突然闪烁
};

