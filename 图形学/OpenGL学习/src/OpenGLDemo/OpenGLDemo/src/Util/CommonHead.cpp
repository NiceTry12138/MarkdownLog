#include "CommonHead.h"

void GLClearError() {
	while (glGetError() != GL_NO_ERROR);
}

void LogError(const char* file, unsigned int Line, const char* functionName) {
	GLuint errorType = glGetError();
	while (errorType != GL_NO_ERROR) {
		std::cout << file << " Line: " << Line << " Function Name: " << functionName << " ";
		switch (errorType)
		{
		case GL_INVALID_ENUM:
			std::cout << "LogError: " << "GL_INVALID_ENUM" << std::endl;
			break;
		case GL_INVALID_VALUE:
			std::cout << "LogError: " << "GL_INVALID_VALUE" << std::endl;
			break;
		case GL_INVALID_OPERATION:
			std::cout << "LogError: " << "GL_INVALID_OPERATION" << std::endl;
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			std::cout << "LogError: " << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
			break;
		case GL_OUT_OF_MEMORY:
			std::cout << "LogError: " << "GL_OUT_OF_MEMORY" << std::endl;
			break;
		//case GL_STACK_UNDERFLOW:
		//	std::cout << "LogError: " << "GL_STACK_UNDERFLOW" << std::endl;
		//	break;
		//case GL_STACK_OVERFLOW:
		//	std::cout << "LogError: " << "GL_STACK_OVERFLOW" << std::endl;
			break;
		}

		// __debugbreak();	// �жϺ��� ������ǿ��غ�����gcc û��

		errorType = glGetError();
	}
}