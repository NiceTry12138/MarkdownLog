#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <string>

#define GL_CHECK_ERROR do { LogError(__LINE__); }while(0);
#define GL_CLEAR_ERROR do { GLClearError(); } while(0);

#define GL_CALL(x) do {			\
	GLClearError();				\
	x;							\
	LogError(__LINE__, #x);		\
} while (0);					\

const int gWidth = 640;
const int gHeight = 480;

// 清除所有错误
void GLClearError();
// 输出当前错误
void LogError(unsigned int Line, const char* functionName = nullptr);
