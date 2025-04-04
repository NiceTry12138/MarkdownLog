#include <iostream>
#include <chrono>

// 先 include glad 再 include glfw
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "Util/RenderSettings.h"

#include "testModule/TestModuleManager.h"


// 定义回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    RSI->ViewportWidth = width;
    RSI->ViewportHeight = height;
}

void InitImGUI(GLFWwindow* window)
{
    // 初始化 Imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // 设置 Imgui 样式
    ImGui::StyleColorsDark();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    const char* glsl_version = "#version 330 core";
    ImGui_ImplOpenGL3_Init(glsl_version);
}

void TestImGUIStyle(GLFWwindow* window)
{
    static float f = 0.0f;
    static int counter = 0;

    ImGui::Begin("Settings");
    ImGui::SliderFloat("Transition", &f, 0.0f, 960.0f);
    if (ImGui::Button("Close Window"))
        glfwSetWindowShouldClose(window, true);
    ImGui::End();
}

void TestRenderImGUI(GLFWwindow* window)
{
    // 设置 ImGUI 新一帧
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    TestImGUIStyle(window);

    // 绘制 ImGUI
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ProcessInput(GLFWwindow* window)
{
    // 监听 ECS 按键
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main()
{
    glfwInit();                                                             // 初始化
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                          // 设置主版本为 3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                          // 设置次版本为 3.  MAJOR + MINOR 得到 3.3 版本
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);          // 使用核心模式 Core-Profile
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);                 // MacOS 系统使用

    GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(RSI->ViewportX, RSI->ViewportY, RSI->ViewportWidth, RSI->ViewportHeight);
    // 绑定回调函数
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    InitImGUI(window);

    auto prevTime = std::chrono::high_resolution_clock::now();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // 如果不需要关闭窗口，则持续进入循环
    while (!glfwWindowShouldClose(window))
    {
        auto currentTime = std::chrono::high_resolution_clock::now();
        float deltaTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - prevTime).count() / 1000.0f;
        prevTime = std::move(currentTime);

        TestModuleManager::GetInstance()->Update(window, deltaTime);

        glfwSwapBuffers(window);    // 交换缓冲区
        glfwPollEvents();           // 检查触发的事件
    }

    //TestApp.OnExit(window);

    glfwTerminate();
	return 0;
}