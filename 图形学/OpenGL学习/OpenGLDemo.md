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

## 渲染流水线

> https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/#_3

![](Image/022.png)

### 顶点着色器

顶点着色器的核心作用之一是将顶点的 3D 坐标 转换为另一种 3D 坐标（通常是 **裁剪空间坐标**）。这个过程涉及多个坐标系的变换：

- 模型空间（Local Space）：顶点在原始模型中的坐标（例如一个立方体的顶点坐标可能是 (-0.5, -0.5, 0.0)）。
- 世界空间（World Space）：通过模型矩阵（Model Matrix）将顶点坐标变换到场景中的全局位置（例如将立方体放置在 (x, y, z) 处）。
- 观察空间（View Space）：通过视图矩阵（View Matrix）将顶点坐标变换到摄像机视角下的坐标系。
- 裁剪空间（Clip Space）：通过投影矩阵（Projection Matrix）将顶点坐标变换到一个规范化空间（范围 [-1, 1]^3），超出范围的顶点会被裁剪掉。

![](Image/023.png)

裁剪剪切体块之外的顶点后，剩余顶点的位置将标准化为称为 NDC（**标准化设备坐标**）的通用坐标系。

标准化设备坐标是一个x、y和z值在-1.0到1.0的一小段空间。任何落在范围外的坐标都会被丢弃/裁剪，不会显示在你的屏幕上

![](Image/024.png)

通过使用由 `glViewport` 函数提供的数据，进行视口变换(`Viewport Transform`)，**标准化设备坐标**(`Normalized Device Coordinates`)会变换为**屏幕空间坐标**(`Screen-space Coordinates`)。所得的屏幕空间坐标又会被变换为片段输入到**片段着色器**中

顶点着色器除了处理位置坐标之外，还能对顶点的其他属性进行初步计算，比如

- 颜色：为顶点赋予颜色
- 纹理坐标：传递或动态生成纹理坐标（UV）
- 法线：对法线进行变化
- 自定义属性：如顶点动画中的位移

顶点着色器只能处理单个顶点的数据，无法直接访问其他顶点的信息

顶点着色器必须输出 `gl_Position` 裁剪空间坐标，其他属性通过 `out` 传递到后续阶段

顶点着色器代码大致如下

```glsl
#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float texIndex;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;

void main() {
	gl_Position = u_MVP * vec4(position, 1.0);
	v_TexCoord = texCoord;
	v_TexIndex = int(texIndex);
	v_Color = inColor;
}
```

### 传入顶点数据

在 [OpenGLStudy](./README.md) 中有详细解释过 VertexArray、VertexBuffer、IndexBuffer，以及如何将数据传递给 OpenGL 以及顶点着色器

### 封装 Shader

[封装Shader](https://learnopengl-cn.github.io/01%20Getting%20started/05%20Shaders/#_6)

### Texture

在 [OpenGLStudy](./src/OpenGLStudy/OpenGLStudy/src/Texture.h) 项目中，有对 `Texture` 贴图的封装

```cpp
void Texture::Init(const std::string& filePath)
{
	m_FilePath = filePath;

	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	GL_CALL(glGenTextures(1, &m_RenderID));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RenderID));

	// 一定要设置的纹理 否则只能得到黑色纹理
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));	// 指定缩小器
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));	// 指定放大器
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	// 传递数据 GL_RGBA8 后面加8 用与表示每个通道站多少位
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));

	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	if (m_LocalBuffer) {
		stbi_image_free(m_LocalBuffer);
		m_LocalBuffer = nullptr;
	}
}
```

这里对贴图进行设置时，涉及到了几个设置 `GL_TEXTURE_WRAP_T`、`GL_TEXTURE_WRAP_S`、`GL_TEXTURE_MAG_FILTER`、`GL_TEXTURE_MIN_FILTER`

根据 `docs.gl` 对 `glTexParameteri` 函数的[解释](https://docs.gl/gl4/glTexParameter)，对于贴图的设置还有很多种

| 贴图设置的参数名称 | 作用 | 可选值 | 默认值 |
| --- | --- | --- | --- | 
| GL_TEXTURE_MIN_FILTER | 设置纹理缩小（远距离）时的采样方式 | GL_NEAREST, GL_LINEAR, GL_NEAREST_MIPMAP_NEAREST, GL_LINEAR_MIPMAP_NEAREST, GL_NEAREST_MIPMAP_LINEAR, GL_LINEAR_MIPMAP_LINEAR | GL_NEAREST_MIPMAP_LINEAR | 
| GL_TEXTURE_MAG_FILTER | 设置纹理放大（近距离）时的采样方式 | GL_NEAREST, GL_LINEAR | GL_LINEAR | 
| GL_TEXTURE_WRAP_S/T/R | 设置纹理坐标在 S/T/R 轴超出 [0,1] 时的处理方式 | GL_REPEAT, GL_MIRRORED_REPEAT, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_BORDER | GL_REPEAT | 
| GL_TEXTURE_BASE_LEVEL | 设置纹理 Mipmap 的最小使用级别（0 表示最高分辨率） | 非负整数 | 0 | 
| GL_TEXTURE_MAX_LEVEL | 设置纹理 Mipmap 的最大使用级别 | 非负整数 | 000（实际由硬件决定） | 
| GL_TEXTURE_MIN_LOD | 设置纹理细节级别（LOD）的最小值 | 浮点数 | 1000（允许最低细节） | 
| GL_TEXTURE_MAX_LOD | 设置纹理细节级别（LOD）的最大值 | 浮点数 | 1000（允许最高细节） | 
| GL_TEXTURE_LOD_BIAS | 设置 LOD 的偏移值（正值为模糊，负值为锐利） | 浮点数 | 0.0 | 
| GL_TEXTURE_COMPARE_MODE | 设置深度/模板纹理的比较模式 | GL_NONE（禁用比较）, GL_COMPARE_REF_TO_TEXTURE（启用比较） | GL_NONE | 
| GL_TEXTURE_COMPARE_FUNC | 设置深度比较函数（需与 GL_COMPARE_REF_TO_TEXTURE 配合使用） | GL_LEQUAL, GL_GEQUAL, GL_LESS, GL_GREATER, GL_EQUAL, GL_ALWAYS 等 | GL_LEQUAL | 
| GL_DEPTH_STENCIL_TEXTURE_MODE | 设置深度/模板纹理的采样模式（返回深度值还是模板值） | GL_DEPTH_COMPONENT, GL_STENCIL_INDEX | GL_DEPTH_COMPONENT | 
| GL_TEXTURE_SWIZZLE_R/G/B/A | 设置纹理颜色通道的映射关系（重新排列或复制通道） | GL_RED, GL_GREEN, GL_BLUE, GL_ALPHA, GL_ZERO, GL_ONE | 各通道对应自身（如 R→R） | 

#### 贴图设置

`GL_TEXTURE_MIN_FILTER` 和 `GL_TEXTURE_MAG_FILTER` 表示贴图放大缩小时，采用的纹理过滤方式

| GL_NEAREST | GL_LINEAR |
| --- | --- | --- |
| 中文 | 临近过滤 | 线性过滤 |
| 作用 | 选择离中心点最接近的纹理坐标的像素 | 基于纹理坐标附近的纹理像素，计算插值 |
| 表现效果 | ![](Image/025.png) | ![](Image/026.png) |

![](Image/027.png)

对比 `GL_NEAREST` 和 `GL_LINEAR` 在图片放大之后的表现效果，很明显发现 `GL_NEAREST` 锯齿感明显，`GL_LINEAR` 更加自然

贴图设置的 `GL_TEXTURE_WRAP_S/T/R` 的 `S/T/R` 对应可以理解为 x、y、z，这个设置的作用就是当纹理坐标超过 `[0, 1]` 的取值范围之后，应该如何处理

![](Image/028.png)

贴图设置的 `GL_TEXTURE_SWIZZLE_R/G/B/A` 用于通道映射

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_A, GL_RED)
```

通过上述代码的设置，读取贴图的 `alpha` 通道的时，从 `r` 通道读取

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_R, GL_RED);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_G, GL_RED);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_B, GL_RED);
```

通过上述代码的设置，读取贴图的 `RGB` 都是通过 `R` 通道获取值，比如 `R` 值为 0.8, `A` 值为 1.0，最后得到的就是 `(0.8, 0.8, 0.8, 1.0)` 的灰度值

#### 贴图的 Mipmap

在游戏运行过程中，物体有远有近

现在有一个大房子，用了一张 1024 * 1024 的贴图，在近距离观看细节丰富，但是远距离一般不需要这么丰富的细节，为了性能考虑可以把贴图换成 512 * 512；如果距离再远一些，即使使用 256 * 256 的贴图效果也不差

为了运行时性能，会根据举例的远近使用不同尺寸的贴图，这就是 `MipMap` **多级渐远纹理**

![](Image/029.png)

在 `OpenGL` 中可以使用 `glGenerateMipmap` 函数来创建多级纹理，而不需要自己手动创建


| 过滤方式 | 描述 | 
| --- | --- |
| GL_NEAREST_MIPMAP_NEAREST | 使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样 | 
| GL_LINEAR_MIPMAP_NEAREST | 使用最邻近的多级渐远纹理级别，并使用线性插值进行采样 | 
| GL_NEAREST_MIPMAP_LINEAR | 在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样 | 
| GL_LINEAR_MIPMAP_LINEAR | 在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样 | 

```cpp
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
```

> 注意，只有在 `GL_TEXTURE_MIN_FILTER` 也就是缩小的时候才设置 `MipMap`，`GL_TEXTURE_MAG_FILTER` 放大的时候设置无效

#### 纹理单元

在 [OpenGLStudy的着色器](./src/OpenGLStudy/OpenGLStudy/res/shader/Fragment.frag) 中使用 `uniform sampler2D u_Texture` 来接收一个贴图

但是在代码中却使用 `glUniform1i` 设置贴图为 0

```cpp
m_shader->SetUniform1i("u_Texture", 0);
```

这是因为，这里设置的 0 并不是指贴图自身，而是贴图的槽位，也被称为 **纹理单元**

```cpp
glActiveTexture(GL_TEXTURE0); // 在绑定纹理之前先激活纹理单元
glBindTexture(GL_TEXTURE_2D, texture);
```

使用 `glActiveTexture` 激活指定的纹理单元之后，使用 `glBindTexture` 会将贴图绑定到当前激活的纹理单元中

`OpenGL` 至少保证有 16 个纹理单元，根据机器不同有的可以支持 32 个，也就是支持 `GL_TEXTURE0` ~ `GL_TEXTURE15`，可以写成 `GL_TEXTURE0 + Index` 的方式

> `GL_TEXTURE8` 等价于 `GL_TEXTURE0 + 8`

#### 封装类

为了方便贴图的使用，封装了下面这样一个简单的 texture 类

```cpp
#include "Texture.h"
#include "stb_image.h"

Texture::~Texture()
{
	DeleteTexture();
}

void Texture::Init(const std::string& filePath)
{
	if (m_FilePath == filePath) {
		return;
	}

	DeleteTexture();
	m_FilePath = filePath;

	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	GL_CALL(glGenTextures(1, & m_TextureId));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, m_TextureId));

	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));		// 指定缩小器
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));		// 指定放大器
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));	// 设置贴图超过 0~1 之后的读取方式
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));	// 

	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));

	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));		// 解绑

	if (m_LocalBuffer) {
		stbi_image_free(m_LocalBuffer);
		m_LocalBuffer = nullptr;
	}
}

void Texture::Bind(GLuint slot)
{
	GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, m_TextureId));
}

void Texture::UnBind()
{
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

int Texture::GetHeight()
{
	return m_Height;
}

int Texture::GetWidth()
{
	return m_Width;
}

void Texture::DeleteTexture()
{
	stbi_image_free(m_LocalBuffer);
	m_LocalBuffer = nullptr;

	GL_CALL(glDeleteTextures(1, &m_TextureId));
	m_TextureId = GL_ZERO;
}

这个类存在一个问题，下面这段代码在运行时会出现问题

```cpp
m_Tex1.Init("res/textures/test2.png");
m_Tex1.Bind(0);
m_Tex2.Init("res/textures/test3.png");
m_Tex2.Bind(1);
```

1. `m_Tex1.Bind(0)` 激活了 `GL_TEXTURE0` 槽位并绑定了贴图
2. 因为当前激活了 `GL_TEXTURE0`，所以`m_Tex2.Init` 仍然在 `GL_TEXTURE0` 槽位上进行
3. `m_Tex2.Init` 最后 `glBindTexture(GL_TEXTURE_2D, 0)` 解绑了 `GL_TEXTURE0` 上的贴图

那么，最后 `m_Tex1` 这个贴图就是绑定失败的，它被 `m_Tex2` 给无意间释放掉了

所以，`Texture` 建议下面这样操作，一起初始化，一起绑定

```cpp
m_Tex1.Init("res/textures/test2.png");
m_Tex2.Init("res/textures/test3.png");
m_Tex1.Bind(0);
m_Tex2.Bind(1);
```

## 坐标系统

`MVP` 矩阵 Module、View、Projection

模型的顶点坐标的原点一般是模型的中心点，通过 Module 矩阵得到顶点的世界坐标

得到世界坐标之后，将其转换成观察空间，也就是以相机为原点的坐标系下

最后通过 Projectin 将顶点坐标进行裁剪，对于空间外的点剔除

### 相机坐标

相机需要定位朝向和坐标，以此来确定视图矩阵

坐标无需多言，由 3 个 float 组成的结构体

```cpp
glm::vec3(0.0f, 0.0f, 3.0f);
```

`OpenGL` 是右手坐标系， X 轴正方向向右，Y轴正方向向上，Z轴正方向垂直屏幕向外

所以，如果想要看到位于（0,0） 坐标的物体，并且相机向后移动的时候，物体缩小，一般是让相机沿着 Z 轴正方向移动

接下来就是确定相机的朝向

通过两个向量就可以确定相机的朝向，一个是 `LoopAt` 一个是 `Up Vector`

确定 `LookAt` 的坐标点，通过 `LookAt` 坐标减去 相机坐标 得到一个方向向量，该向量就是相机朝向，通过这个向量可以确定相机朝向中的 `Yaw` 和 `Pitch`

`Up Vector` 表示相机向上的向量，通过这个向量，确定相机朝向的 `Rall`

不过有的时候，也可以需要知道相机的 `Right Vector`，不过这个比较好计算，通过 `Look` 和 `Up Vector` 通过**叉乘**便可以计算出 `Right Vector`

```cpp
// 计算 LookAt Vector
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);

// 定义 Up Vector
glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

// 计算 Right Vector
glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
```

通过上面计算的向量和坐标，可以计算得到 View 矩阵

```cpp
glm::mat4 view;
						// 相机坐标					 	目标坐标							Up Vector
view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f),  glm::vec3(0.0f, 0.0f, 0.0f),  glm::vec3(0.0f, 1.0f, 0.0f));
```

记住，`OpenGL` 的坐标系是右手坐标系，y轴向上，x轴向左，z轴垂直屏幕向外

![](Image/030.png)

所以 `direction.y = sin(pitch)`

![](Image/031.png)

在计算 `xz` 轴是，向量长度是 `cos(pitch)` 

对应的点的坐标是 

- `direction.x = cos(pitch) * cos(yaw)`
- `direction.z = cos(pitch) * sin(yaw)`

注意：此时 `Camera Up Vector` 保持不变，因为 `Up Vector` 是用于控制 `Roll` 的，一般不会变化

## 颜色和光照

人能观察到物体的颜色，其实是物体不能吸收的颜色，也可以理解为反射的颜色

![](Image/032.png)

如果一个物体是白色，那么它对 RGB 三通道的颜色的吸收率是 0%，也就是 RGB 的反射率是 100%，所以它的 RGB 值为 (1.0, 1.0, 1.0)

如果一个物体是黑色，那么它对 RGB 三通道的颜色的吸收率是 100%，也就是 RGB 的反射率是 0%，所以它的 RGB 值为 (0.0, 0.0, 0.0)

如果一个物体是正红色，那么它对 RGB 三通道的颜色的吸收率是 0%，100%，100%，所以它的 RGB 值为 (1.0, 0.0, 0.0)

如果一个红色光打在蓝色的物体上，这个物体显示的颜色应该是黑色，光的颜色是 (1, 0, 0)，物体是蓝色，的三通道的反射率是 (0, 0, 1)，相乘得到的结果是 (0, 0, 0)

推理得到下面的基本光照计算

```cpp
glm::vec3 lightColor(l1, l2, l3);	// l1、l2、l3 光照的 RGB 值 
glm::vec3 toyColor(t1, t2, t3);		// t1、t2、t3 物体的 RGB 值
glm::vec3 finalColor = lightColor * toyColor;
```

现实生活中，光照是很复杂的，通过简化的光照模型可以用较少的计算量模拟出现实光照

使用的较多的简单光照模型就是 Phong 模型，它包括：环境光、漫反射光、镜面反射光

- 环境光照( `Ambient` )：即使在黑暗的情况下，通常也有一些光亮（月光、星光等），所以物体通常不是完全黑暗的，为了模拟这个设置了环境光照，他永远会给物体一些颜色
- 漫反射光照( `Diffuse` )：模拟光源对物体的方向性影响
- 镜面光照( `Specular` )：模拟光泽物体上出现的亮点

![](Image/033.png)

[101中关于反射的计算](../Games101/图形学.md#blinn-phong反射模型)

根据漫反射、镜面反射和环境光的计算，得到着色器内容如下

- 顶点着色器

通过 `GL_ARRAY_BUFFER` 获得顶点坐标和法线，通过 `uniform` 设置 MVP 矩阵，计算得到顶点的直接坐标 `FragPos`

```cpp
#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 FragPos;

void main() {
	gl_Position = projection * view * model * vec4(inPosition, 1.0);

	Normal = inNormal;
	FragPos = vec3(model * vec4(inPosition, 1.0));
}
```

- 片段着色器

通过 `uniform` 设置相机坐标和灯光坐标、灯光颜色、物体颜色等参数

通过顶点着色器得到顶点的世界坐标、顶点法线

```cpp
#version 330 core

layout(location = 0) out vec4 o_color;

in vec3 FragPos;                // 顶点坐标
in vec3 Normal;					// 法线向量

uniform vec3 viewPos;           // 相机坐标
uniform vec3 lightPos;          // 灯的坐标

uniform vec3 objectColor;       // 基本颜色
uniform vec3 lightColor;        // 灯光颜色

uniform float ambientStrength;  // 环境光强度
uniform float diffuseStrength;  // 漫反射强度
uniform float specularStrength;	// 镜面反射强度

void main() {
	// 环境光
	vec3 ambient = ambientStrength * lightColor;
	
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	// 光的强度与距离有一定关系
	float lightDis = distance(lightPos, FragPos);
	float disRate = 1 / lightDis / lightDis;

	// 漫反射
	float rate = max(dot(lightDir, normal), 0.0);	// 因为光线可能从物体的反面，此时忽略这个，所以用 max(0.0, )
	vec3 diffuse = disRate * diffuseStrength * rate * lightColor;

	// 镜面反射
	vec3 enterViewDir = viewPos - FragPos;
	vec3 halfV = (lightDir + enterViewDir) / length(lightDir + enterViewDir);
	halfV = normalize(halfV);						// 半程向量
	float specularRate = pow(max(dot(halfV, normal), 0.0), 32);
	vec3 specular = disRate * specularStrength * specularRate * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	o_color = vec4(result, 1.0);
}
```

跟之前不同的是，这里修改了 `Vertex` 的结构体

```cpp
// 顶点信息 v1 版本 
struct Vertex_v1
{
	float position[3];			  // 顶点坐标
	float normal[3];			  // 法线贴图
};
```

![](Image/034.png)

物体具体是什么颜色，可以通过贴图来表现，物体的高光颜色强度也可以通过贴图来表示

| 物体漫反射颜色 | 物体镜面反射颜色 |
| --- | --- | 
| ![](Image/035.png) | ![](Image/036.png) |

在片段着色器中，可以使用 UV 坐标获取贴图颜色，将漫反射颜色 + 镜面反射颜色 得到的就是最后物体表现的颜色

```cpp
// 通过贴图 漫反射颜色
vec3 diffuse = light.diffuse * diff * texture(cubeMaterial.diffuse, TexCoords).rgb;
// 通过贴图 镜面反射颜色  
vec3 specular = light.specular * spec * texture(cubeMaterial.specular, TexCoords).rgb;  
```

![](Image/037.png)

观察箱子的表面

- 在镜面反射贴图中间是一片黑色，所以物体中间几乎没有镜面反射的高光
- 在镜面反射贴图四周是带有花纹的白色，所以物体四周会有不规则的高光，模拟出磨损的效果

### 光的种类

1. 平行光

对于现实的参考就是阳光，由于距离过远可以直接理解为平行光

对于这种光，无法也不需要定义坐标，只需要定义一个向量，表示光的方向即可

```cpp
struct Light {
    // vec3 position; // 使用定向光就不再需要了
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};
```

2. 点光源

对于现实的参考就是白炽灯，具有坐标，且向四周发出光亮

对于点光源，至少需要包含光源坐标，其实之前的案例就是几个最简单的点光源

除了坐标之外，通常会设置距离。目前常见的点光源都有两个距离：**开始衰减距离**和**最远光照距离**

显示生活中，灯光越近越亮，越远越按。同理，在虚拟世界也可以这么设置，**开始衰减距离**表示灯光强度开始衰减，**最远光照距离** 表示在此之外的物体不受该灯光影响

![](Image/038.png)

很明显中间这段弧线是个反函数，可以通过设置参数来控制函数曲线

```cpp
struct Light {
    vec3 position;  

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;
};
```

> 计算例子： 1.0 / (constant + linear * distance + quadratic * distance * distance)

- 聚光

对于现实的参考就是聚光灯，具有坐标，和一个**特定方向**的照射光线

与点光源相似，有一个**开始衰减**的角度和一个照射的**最大角度**

![](Image/039.png)

| 变量定于 | 作用 |
| --- | --- 
| LightDir | 从片段指向光源的向量 |
| SpotDir | 聚光灯所指向的方向 |
| Phi | 聚光半径的切光角 |
| Theta | LightDir 和 SpotDir 向量之间的夹角 |

## 模型

使用 `assimp` 来加载模型
需要添加 `include` 和 `lib` 的路径

```bash
$(SolutionDir)Dependencies\assimp\include
$(SolutionDir)Dependencies\assimp\lib
```

需要添加 `assimpd.lib` 库

并且在生成事件中，将 `assimpd.dll` 复制到输出路径

```bash
copy /Y "$(SolutionDir)Dependencies\assimp\lib\assimpd.dll" "$(OutDir)"
```

![](Image/040.png)

> 编译时依赖 `assimpd.lib`，运行时依赖 `assimpd.dll`

编译 `win32` 平台的 `assimp` 使用命令行

```bash
mkdir build && cd build
cmake ../ -A win32
```

## 高级 OpenGL

### Blender

[在OpenGL学习的ReadMe.md中有解释过Blend](README.md/#关于混合)

这里需要重复强调的是，渲染顺序问题

由于开启了深度缓冲，如果存在两个半透明物体，A 离相机近，B 离相机远

- 先渲染 B 后渲染 A
  - 渲染 A 时由于深度缓冲的存在，将原本的颜色和 A 的颜色进行混合，然后覆盖深度缓冲
- 先显然 A 后渲染 B
  - 显然 B 时由于深度缓冲的存在，直接丢弃 B 片段的颜色，导致渲染结果错误

所以，在渲染时

1. 渲染所有不透明的物体
2. 对所有的透明物体进行排序，从远到近
3. 按顺序绘制所有透明物体

### 面剔除


### 帧缓冲

详见 [TestFrameBuffer.cpp](./src/OpenGLDemo/OpenGLDemo/src/testModule/TestFrameBuffer.cpp)

### 立方体贴图

立方体贴图就是一个包含了6个2D纹理的纹理，每个2D纹理都组成了立方体的一个面

立方体贴图有一个非常有用的特性，它可以通过一个方向向量来进行索引/采样

![](Image/041.png)

立方体贴图有一个经常使用的场景：**天空盒**

**天空盒** 顾名思义就是其实是将世界包围在一个巨大的盒子里面，然后通过当前相机朝向作为向量计算与立方体贴图交点，得到背景颜色信息

在绘制天空盒的时候，消除相机位移的影响，保证天空盒的中心永远在相机上，这样子就能保证天空盒不会由于相机移动导致偏移

对比将天空盒放大到特别大来模拟效果，首先过远的平面会被剔除，其次相机一直移动总归是能够移动到边界的

```cpp
// 通过将 4阶矩阵 转换为 3阶矩阵 去除位移信息，保证天空盒永远以相机为中心
auto cubeView = glm::mat4(glm::mat3(view));
```

绘制天空盒需要保证天空盒的深度永远是最底层的，有两种方案

1. 绘制天空盒时关闭深度写入，这样天空盒就会永远绘制带其他物体之后，虽然它离我们很近，但是实际的深度却很高
2. 根据深度计算的公式，将天空盒顶点的计算值永远设置为 1，那么其深度永远低于其他

前面写过顶点着色器，顶点其实是一个四维向量 (x、y、z、w)，顶点的 NDC 坐标其实 (x/w, y/w, z/w) 

所以，如果将天空盒的顶点 z 轴手动设置为 w，那么天空盒深度永远是 1，永远在其他物体的后面

```glsl
vec4 pos = projection * view * vec4(inPosition, 1.0);
gl_Position = pos.xyww;
```

### 高级数据

使用 `glBufferData` 会分配一块 GPU 内存，并将数据添加到这块内存中，如果设置为 null 则只会分配，不会填充

使用 `glBufferSubData` 可以填充缓冲的特定区域

`glBufferSubData` 函数定义如下

```cpp
void glBufferSubData(	GLenum target,
						GLintptr offset,
						GLsizeiptr size,
						const GLvoid * data);
```

`glBufferSubData` 函数使用如下

```cpp
// 创建并初始化顶点缓冲区
GLuint VBO;
glGenBuffers(1, &VBO);
glBindBuffer(GL_ARRAY_BUFFER, VBO);
float vertices[12] = { /* 初始6个顶点（每个含x,y） */ };
glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);

// 更新第3到第4个顶点（索引2和3）
float updatedData[4] = {0.5f, 0.5f, -0.5f, -0.5f}; // 新坐标 (x1,y1, x2,y2)
glBufferSubData(GL_ARRAY_BUFFER, 2 * sizeof(float) * 2, sizeof(updatedData), updatedData);
```

使用 `glMapBuffer` 可以得到缓冲内存的指针，然后通过 `memcpy` 拷贝数据进去

```cpp
float data[] = {
  0.5f, 1.0f, -0.35f
  ...
};
glBindBuffer(GL_ARRAY_BUFFER, buffer);
// 获取指针
void *ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
// 复制数据到内存
memcpy(ptr, data, sizeof(data));
// 记得告诉OpenGL我们不再需要这个指针了
glUnmapBuffer(GL_ARRAY_BUFFER);
```

这里的 `glUnmapBuffer` 不是丢弃 `GL_ARRAY_BUFFER`，而是接触指针映射，即 `ptr` 指针无法映射到 GPU 内存中，继续使用 `ptr` 会导致错误

> 相当于 `glUnmapBuffer` 会释放 CPU 对缓冲区内存的映射权限

使用 `glCopyBufferSubData` 能够将一个缓冲目标的数据拷贝到另一个缓冲目标中

```cpp
void glCopyBufferSubData(	GLenum readTarget,
							GLenum writeTarget,
							GLintptr readOffset,
							GLintptr writeOffset,
							GLsizeiptr size);
```

> 函数定义

```cpp
// 创建两个缓冲区
GLuint vboSrc, vboDst;
glGenBuffers(1, &vboSrc);
glGenBuffers(1, &vboDst);

// 填充源缓冲区
float srcData[] = { /* 顶点数据 */ };
glBindBuffer(GL_COPY_READ_BUFFER, vboSrc);
glBufferData(GL_COPY_READ_BUFFER, sizeof(srcData), srcData, GL_STATIC_DRAW);

// 初始化目标缓冲区（大小与源相同）
glBindBuffer(GL_COPY_WRITE_BUFFER, vboDst);
glBufferData(GL_COPY_WRITE_BUFFER, sizeof(srcData), NULL, GL_STATIC_DRAW);

// 复制整个缓冲区
glCopyBufferSubData(
    GL_COPY_READ_BUFFER,
    GL_COPY_WRITE_BUFFER,
    0, 0, sizeof(srcData)
);

// 使用目标缓冲区渲染
glBindBuffer(GL_ARRAY_BUFFER, vboDst);
glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
glDrawArrays(GL_TRIANGLES, 0, 36);
```
> 使用案例

### 高级 GLSL

`Uniform` 缓冲对象，按照之前写过的着色器来看，我们需要对 模型、灯光、天空盒 等物体的着色器设置 model、view、projection 三个矩阵，但是 view 和 projection 矩阵都是相同的

为了避免重复设置， `OpenGL` 提供了 Uniform 缓冲对象

首先需要在 着色器中定义 Uniform 块

```glsl
// GLSL 着色器中定义的 Uniform 块
layout(std140) uniform MyUBO {
    mat4 viewMatrix;
    mat4 projectionMatrix;
    vec3 lightPosition;
    float lightIntensity;
};
```

Uniform 缓冲对象(Uniform Buffer Object) 也是一种缓冲，使用 `glGenBuffer` 来创建它

```cpp
GLuint ubo;
glGenBuffers(1, &ubo);
glBindBuffer(GL_UNIFORM_BUFFER, ubo);

// 根据 MyUBO 的定义 计算总大小（需手动对齐）
const GLsizeiptr uboSize = 
    sizeof(glm::mat4) * 2 +  // viewMatrix + projectionMatrix
    sizeof(glm::vec4) +      // lightPosition（vec3 按 vec4 对齐）
    sizeof(float);           // lightIntensity

glBufferData(GL_UNIFORM_BUFFER, uboSize, NULL, GL_DYNAMIC_DRAW);
```

使用之前提到过的 `glMapBuffer` + `memcpy` 来填入数据

```cpp
// 假设使用 glm 数学库
glm::mat4 view = ...;
glm::mat4 projection = ...;
glm::vec3 lightPos = ...;
float intensity = ...;

// 映射 UBO 并写入数据
glBindBuffer(GL_UNIFORM_BUFFER, ubo);
GLvoid* ptr = glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);

// 写入 viewMatrix（偏移 0）
memcpy(ptr, &view, sizeof(glm::mat4));

// 写入 projectionMatrix（偏移 sizeof(mat4)）
memcpy((char*)ptr + sizeof(glm::mat4), &projection, sizeof(glm::mat4));

// 写入 lightPosition（偏移 sizeof(mat4)*2，按 vec4 对齐）
glm::vec4 lightPosAligned(lightPos, 0.0f); // 填充为 vec4
memcpy((char*)ptr + sizeof(glm::mat4)*2, &lightPosAligned, sizeof(glm::vec4));

// 写入 lightIntensity（偏移 sizeof(mat4)*2 + sizeof(vec4)）
memcpy((char*)ptr + sizeof(glm::mat4)*2 + sizeof(glm::vec4), &intensity, sizeof(float));

glUnmapBuffer(GL_UNIFORM_BUFFER);
```

与 `Texture` 类似，将 Uniform Buffer Object 绑定到指定的槽上

```cpp
// 将 UBO 对象关联到绑定点 0
glBindBufferBase(GL_UNIFORM_BUFFER, 0, ubo);

// 对 shaderProgramA
GLuint uboIndexA = glGetUniformBlockIndex(shaderProgramA, "MyUBO");
glUniformBlockBinding(shaderProgramA, uboIndexA, 0); // 绑定到绑定点 0

// 对 shaderProgramB
GLuint uboIndexB = glGetUniformBlockIndex(shaderProgramB, "MyUBO");
glUniformBlockBinding(shaderProgramB, uboIndexB, 0); // 同样绑定到绑定点 0
```

这里有一个点需要注意到，那就是着色器中定义 Uniform 块时使用的 `std140`

`std140` 是 OpenGL 中用于定义 Uniform 缓冲对象(Uniform Buffer Object) 和 着色器缓冲对象(SSBO) **内存布局**的规则

目的是**显式控制**，使得 CPU 和 GPU 能够正确读取数据，避免因为硬件差异导致的数据错位


