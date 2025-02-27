# OpenGl Demo

- 配合 src/OpenGLDemo 项目使用
- 配合 [learn-opengl-cn](learnopengl-cn.github.io) 使用

## 创建项目

这次使用的是 GLFW 和 GLAD

- `GLFW` 管理窗口和事件
- `GLAD` 管理版本

### 初始化项目

```cpp
glfwInit();                                                             // 初始化
glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);                          // 设置主版本为 3
glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);                          // 设置次版本为 3.  MAJOR + MINOR 得到 3.3 版本
glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);          // 使用核心模式 Core-Profile
// 在 macOS 上启用向前兼容（Forward Compatibility），确保代码符合苹果平台的严格限制
// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);                 // MacOS 系统使用 
```

`GLFW_OPENGL_PROFILE` 设置 OpenGL 使用模式：**核心模式** or **兼容模式**

- **核心模式**
  - 3.2 之后引入的上下文模式
  - 完全移除旧版 OpenGL 的遗留功能（如固定管线函数 glBegin/glEnd、立即模式等）
  - 所有渲染必须通过可编程管线（Shader）完成，强制开发者使用现代 OpenGL 的最佳实践（如 VBO、VAO、着色器等）
- **兼容模式**
  - 允许新旧 OpenGL API 混合使用，保留传统功能（如固定管线），但可能导致代码臃肿和性能损失
  - 通常不建议使用，除非需要维护旧代码

一般需要显示设置目标模式，部分驱动可能默认选择兼容模式

### 创建窗口视口

使用 `glfwCreateWindow` 创建窗口，使用 `glfwMakeContextCurrent` 设置窗口环境上下文，使用 `glViewport` 设置视口大小

```cpp
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

glViewport(0, 0, 800, 600);
```

需要明确的是 **窗口** 和 **视口** 不是同一个东西


| 特性 | 窗口（Window） | 视口（Viewport） | 
| --- | --- | --- |
| 创建者 | 操作系统（通过 GLFW 创建） | OpenGL（通过 glViewport 设置） | 
| 作用 | 显示图形、接收输入事件 | 定义渲染内容在窗口中的显示区域 | 
| 坐标系 | 屏幕像素坐标（如 (0,0) 是左上角或左下角） | 相对于窗口的像素坐标（左下角为原点） | 
| 动态调整 | 用户可拖动窗口改变大小 | 需手动调用 glViewport 更新 | 
| 多视口支持 | 一个窗口只能有一个 | 一个窗口可以设置多个视口（多视角渲染） | 

这里只是恰好 `glfwCreateWindow` 创建的窗口和  `glViewport` 创建的视口大小一样

- `glViewport` 前两个参数 (x, y) 表示视口左下角相对窗口左下角的位置偏移
- `glViewport` 后两个参数 (width, height) 表示视口宽高(像素)

在窗口大小改变的时候，通常期望修改视口大小，可以通过注册回调函数来实现这个功能

```cpp
// 定义回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// 绑定回调函数
glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
```

在使用 `glViewPort` 之前，需要先初始化 `glad`

`glad` 用于管理 `OpenGL` 的函数指针，所以在使用 `OpenGL` 的函数之前，一定要先初始化 `glad`

在 glfw、flad、窗口、视口 都创建完毕之后，就可以开始**渲染循环**(`Render Loop`)

```cpp
// 每帧检查是否需要关闭窗口，不需要则进入渲染循环
while (!glfwWindowShouldClose(window))
{
    
    glfwSwapBuffers(window);    // 交换缓冲区
    glfwPollEvents();           // 检查触发的事件
}
```

`glfwSwapBuffers` 用于交换缓冲区，那么什么是缓冲区？为什么需要交换缓冲区？

缓冲区用于存储绘制生成的图像

由于图片不是一次性绘制好，而是一个像素一个像素的从左到右从上到下的绘制出来的，如果使用单缓冲区很容易出现图像闪烁的问题，并且由于最终图像不是瞬间显示出来，可能会导致渲染结果不真实

为了规避上面说的问题，一般采用双缓冲渲染窗口应用程序。**前缓冲**保存着最终输出的图像，所有渲染指令都在**后缓冲**上绘制

当所有渲染指令执行完毕之后，直接交换(`swap`)前后缓冲区，就能立刻得到最终图片，所以这个函数叫 `glfwSwapBuffers`

> `OpenGL` 默认启用双缓冲，可以通过 `glGetIntegerv` 函数检查 `GL_DOUBLEBUFFER` 的值是否为 1

当关闭窗口，准备退出程序的时候，需要释放/删除分配的所有资源，也就是在 `main` 函数退出的地方调用 `glfwTerminate` 

```cpp
glfwTerminate();
return 0;
```

### 输入和渲染

使用 `glfwGetKey` 可以通过 `window` 判断是否触发按键

```cpp
void ProcessInput(GLFWwindow* window)
{
    // 监听 ECS 按键
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
```

使用 `glClear` 来清空屏幕，可以通过 `glClearColor` 设置清空屏幕所用的颜色

```cpp
// 如果不需要关闭窗口，则持续进入循环
while (!glfwWindowShouldClose(window))
{
    ProcessInput(window);       // 检查按键触发

    // 清空屏幕颜色 防止上一帧的内容影响这一帧的内容
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // TestRenderImGUI(window); // ImGUI 测试 不用管

    glfwSwapBuffers(window);    // 交换缓冲区
    glfwPollEvents();           // 检查触发的事件
}
```

