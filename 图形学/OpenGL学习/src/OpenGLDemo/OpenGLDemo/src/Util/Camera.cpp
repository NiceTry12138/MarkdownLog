#include "Camera.h"

void Camera::InputProcess(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
	{
		m_CameraLocation -= glm::normalize(glm::cross(m_CameraForward, m_CameraUp)) * m_MoveSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
	{
		m_CameraLocation += glm::normalize(glm::cross(m_CameraForward, m_CameraUp)) * m_MoveSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
	{
		m_CameraLocation += m_CameraForward * m_MoveSpeed;
	}
	else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
	{
		m_CameraLocation -= m_CameraForward * m_MoveSpeed;
	}
}

void Camera::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{
	if (m_isFirstMouse)
	{
		m_LastMousePosX = xpos;
		m_LastMousePosY = ypos;
		m_isFirstMouse = false;
	}

	float offsetX = xpos - m_LastMousePosX;
	// OpenGL 屏幕空间 Y 轴正方向向下，所以向下移动时 ypos 会增加
	float offsetY = -(ypos - m_LastMousePosY);

	m_LastMousePosY = ypos;
	m_LastMousePosX = xpos;

	offsetX *= m_RorateSpeed;	// 比例变换
	offsetY *= m_RorateSpeed;

	m_Yaw += offsetX;
	m_Pitch += offsetY;

	// 限制 pitch 仰角的大小，你头向上仰也有一个限制 类似的
	if (m_Pitch > 89.0f || m_Pitch < -89.0f)
	{
		m_Pitch = glm::clamp(m_Pitch, -89.0f, 89.0f);
	}

	glm::vec3 front;
	front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	front.y = sin(glm::radians(m_Pitch));
	front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_CameraForward = glm::normalize(front);
}

glm::mat4 Camera::GetView()
{
	return glm::lookAt(m_CameraLocation, m_CameraLocation + m_CameraForward, m_CameraUp);
}

void Camera::SetFirstMouse(bool bIsFirst)
{
	m_isFirstMouse = bIsFirst;
}

void Camera::SetMoveSpeed(float moveSpeed)
{
	m_MoveSpeed = moveSpeed;
}

void Camera::SetRotateSpeed(float rotateSpeed)
{
	m_RorateSpeed = rotateSpeed;
}

void Camera::SetLocation(glm::vec3 inLocation)
{
	m_CameraLocation = inLocation;
}

glm::vec3 Camera::GetCameraLocation()
{
	return m_CameraLocation;
}
