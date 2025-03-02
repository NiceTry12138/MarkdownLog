#pragma once

#include <iostream>
#include <string>
#include <chrono>
#include <map>
#include <unordered_map>
#include <set>
#include <unordered_set>
#include <vector>
#include <algorithm>

#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include "glm/gtc/matrix_transform.hpp"

#define GL_CHECK_ERROR do { LogError(__LINE__); }while(0);
#define GL_CLEAR_ERROR do { GLClearError(); } while(0);

#define GL_CALL(x) do {						\
	GLClearError();							\
	x;										\
	LogError(__FILE__, __LINE__, #x);		\
} while (0);								\

// 清除所有错误
void GLClearError();
// 输出当前错误
void LogError(const char* file, unsigned int Line, const char* functionName = nullptr);