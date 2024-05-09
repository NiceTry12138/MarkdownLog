# OpenGL

OpenGL（Open Graphics Library）是一个用于渲染2D和3D矢量图形的跨语言、跨平台的应用程序编程接口（API）。这个API由近350个不同的函数调用组成，可以用来绘制从简单的图形到复杂的三维景象。OpenGL不仅是一个规范，它定义了一系列操作图形和图像的函数，但本身并不提供API的实现。这些实现通常被称为“驱动”，由GPU的硬件开发商提供，负责将OpenGL定义的API命令翻译为GPU指令

`OpenGL` 函数功能、参数查询网站 [docs.gl](https://docs.gl/)

## 创建窗口

### GLFW

使用 `glfw` 来学习 `OpenGL` 的 API， 网址 [www.flgw.org](https://www.glfw.org)，以及[示例代码](https://www.glfw.org/documentation.html)

GLFW 和 OpenGL 之间的关系是密切且互补的。GLFW 是一个专门为 OpenGL 设计的库，它提供了创建窗口、处理输入和事件的简单API，而 OpenGL 负责通过这些窗口进行 2D 和 3D 图形的渲染

具体来说，OpenGL 是一个底层的图形渲染规范，它定义了一系列的 API 来直接与图形硬件交互。这些 API 允许开发者在不同的平台和设备上创建复杂的图形渲染效果。然而，OpenGL 本身并不处理窗口创建或输入事件，这就是 GLFW 发挥作用的地方

总的来说，GLFW 作为一个辅助库，它简化了 OpenGL 程序的窗口和事件管理，让开发者能够更容易地创建跨平台的图形应用程序。而 OpenGL 则专注于提供强大的图形渲染能力。两者结合使用，可以创建功能丰富、效果出色的图形应用程序

GLFW 提供了一种标准化的方法来创建和管理窗口，以及处理键盘、鼠标等输入设备的事件。这使得开发者可以专注于 OpenGL 渲染，而不必担心操作系统特定的窗口管理细节。此外，GLFW 还支持多平台，这意味着你可以在 Windows、macOS 和 Linux 上使用相同的代码来管理窗口和事件

首先使用 `visual studio` 创建一个空项目

将 `GLFW` 的头文件添加到索引路径中，在项目 `sln` 文件同级目录中创建 `Dependencies\GLFW` 文件夹，将下载的 `GLFW` 头文件和对应的链接库拷贝到新建文件夹中 

![](Image/001.png)

> `$(SolutionDir)` 是 `visual studio` 提供的**宏**，表示当前项目所在路径

与添加头文件路同理，添加链接库路径

![](Image/002.png)

在输入中指明使用 `glfw3.lib` 和 `opengl32.lib`，因为 `GLFW` 中提供了多个 `lib` 和 `dll`，所以指定使用一个

> 一般来说系统路径中有 `opengl32.lib` 所以可以直接输入

![](Image/003.png)

| 文件名 | 作用 |
| --- | --- |
| `glfw3.dll`  | 一个动态链接库（`DLL`），它包含 `GLFW` 的编译代码。当你的应用程序运行时，它会动态地从这个 `DLL` 加载代码。这意味着你可以更新 GLFW 版本而不需要重新编译你的应用程序 |
| `glfw3.lib` 和 `glfw3_mt.lib` | 静态链接库，它们包含了编译后的 `GLFW` 代码，可以直接链接到你的应用程序中。这样做的好处是你的应用程序不需要在运行时加载 `DLL`，因为所有的代码都已经包含在应用程序的二进制文件中了。`glfw3_mt.lib` 特别指的是多线程版本的静态库 |
| `glfw3dll.lib` | 一个导入库（`import library`），它用于与 `glfw3.dll` 动态链接。如果你选择使用 `DLL` 版本的 `GLFW`，你的应用程序会在编译时链接到这个导入库，而在运行时从 `glfw3.dll` 加载实际的函数实现 |

> 总的来说，`.lib` 文件用于静态链接，而 `.dll` 文件用于动态链接。选择哪种方式取决于你的具体需求和你的项目配置

这里将项目设置为 x86 架构，因为我所有的配置设置的平台也是 `Win32`， GLFW 下载的也是 32 位版本

![](Image/004.png)

直接运行就可以创建一个空白窗口

测试一下绘制一个三角形 

```cpp
glBegin(GL_TRIANGLES);
glVertex2f(-0.5f, -0.5f);
glVertex2f(0.0f, 0.5f);
glVertex2f(0.5f, -0.5f);
glEnd();
```

### GLEW

除了 GLFW 之外， OpenGL 还有一些其他的辅助库

| 库名称 | 功能 |
| --- | --- |
| GLEW (OpenGL Extension Wrangler Library) | GLEW 是用于管理OpenGL扩展的库，它能自动识别并加载当前平台所支持的全部OpenGL高级扩展函数。适用于需要使用OpenGL 2.0及以上版本的高级特性的场景 |
| GLAD | GLAD是GLEW的现代替代品，也是一个OpenGL扩展加载库。它允许开发者指定需要的OpenGL版本，并根据这个版本加载所有相关的OpenGL函数。适用于需要跨平台支持和使用最新OpenGL特性的项目 |
| GLUT (OpenGL Utility Toolkit) | GLUT是一个较老的工具库，用于创建窗口、处理输入和事件。由于其年代久远且不再维护，通常不推荐用于新项目 |
| FreeGLUT | FreeGLUT是GLUT的开源替代品，完全兼容GLUT。它提供了GLUT缺少的一些功能和修复了一些已知的bug。适用于需要GLUT功能但希望有更好支持和更新的项目 |
| GLFW | GLFW是一个用于创建窗口、读取输入、处理事件的多平台库，支持OpenGL、OpenGL ES和Vulkan。它是GLUT和FreeGLUT的现代替代品，提供了更简洁的API和更好的性能。适用于需要轻量级窗口和事件管理的现代OpenGL应用程序 |

因为只是学习使用 OpenGL，所以直接用 `GLEW` 就行了，[官网下载即可](https://glew.sourceforge.net/)，下载当然是下载 `Binaries`，如果是正规项目最好是下载 `Source` 源码版，源码版断点调试更方便

使用 `GLEW` 的原因是，`Windows` 默认只支持到 **OpenGL 1.1** 的函数，而现代 `OpenGL` 已经发展到更高的版本。为了使用这些高级特性，就必须通过扩展来获取新的功能。GLEW库就是为了解决这个问题而设计的。它允许开发者在不同的平台上使用高版本的 `OpenGL` 功能，而无需担心具体的扩展加载细节。在不使用 `GLEW` 的情况下，开发者需要手动加载每个扩展函数的地址，这是一个繁琐且容易出错的过程。`GLEW` 自动处理这一过程，简化了开发工作。

与之前导入 `GLFW` 库一样，将 `GLEW` 的头文件和链接库导入并且配置到项目中

动态链接库使用 `lib\Release\Win32\glew32s.lib` 文件

| 文件名 | 作用 |
| --- | --- |
| glew32s.lib | 静态链接库的版本，其中的 `s` 表示静态（`static`）。使用这个版本的库时，GLEW的代码会被直接链接到最终的应用程序中，因此不需要在运行时提供 glew32.dll 文件。这可以使得应用程序的分发更加简单，因为不需要额外的DLL文件 |
| glew32.lib | 用于动态链接的导入库。当你使用这个库时，你的应用程序在运行时会依赖 glew32.dll 动态链接库。这意味着，glew32.dll 必须在应用程序运行时可用，通常是放在应用程序的同一目录或系统路径中 |

- 如果希望应用程序能够通过替换DLL来更新GLEW版本，或者减小最终可执行文件的大小，选择动态链接的 `glew32.lib`
- 如果希望简化应用程序的部署，并且不介意增加可执行文件的大小，那么静态链接的 `glew32s.lib` 可能是更好的选择

> 方便使用，直接用 `glew32.lib` 文件

关于 `GLEW` 的使用可以查看它的[文档](https://glew.sourceforge.net/basic.html)

**启动 GLEW**

1. 首先，需要创建一个有效的OpenGL渲染上下文，并调用glewInit()来初始化扩展入口点
2. 如果glewInit()返回GLEW_OK，那么初始化成功，可以使用可用的扩展以及核心OpenGL功能

```cpp
#include <GL/glew.h>
#include <GL/glut.h>
// ...
glutInit(&argc, argv);
glutCreateWindow("GLEW Test");
GLenum err = glewInit();
if (GLEW_OK != err)
{
  /* Problem: glewInit failed, something is seriously wrong. */
  fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
  // ...
}
fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
```

------------------

**检查扩展**

从GLEW 1.1.0开始，你可以通过查询全局定义的变量 `GLEW_{extension_name}` 来找出特定扩展是否在你的平台上可用

```cpp
// 在这里使用ARB_vertex_program扩展是安全的。
if (GLEW_ARB_vertex_program)
{
  glGenProgramsARB(...);
}
```

可以检查核心OpenGL功能。例如，要看 `OpenGL 1.3` 是否支持

```cpp
if (GLEW_VERSION_1_3)
{
  // OpenGL 1.3被支持！
}
```

也可以从字符串输入执行扩展检查。从 **1.3.0** 版本开始，使用 `glewIsSupported` 来检查所需的核心或扩展功能是否可用

```cpp
if (glewIsSupported("GL_VERSION_1_4  GL_ARB_point_sprite"))
{
  // 有OpenGL 1.4 + 点精灵
}
```

对于仅限扩展的情况，`glewGetExtension` 提供了一个较慢的替代方法（GLEW 1.0.x-1.2.x）。请注意，在 **1.3.0** 版本中 `glewGetExtension` 被 `glewIsSupported` 替代

```cpp
if (glewGetExtension("GL_ARB_fragment_program"))
{
  // ARB_fragment_program是被支持的
}
```

------------------

**实验性驱动程序**

`GLEW` 从图形驱动程序获取支持的扩展信息。然而，实验性或预发布的驱动程序可能不会通过标准机制报告每个可用的扩展，在这种情况下，`GLEW` 会报告它不支持。为了规避这种情况，可以在调用 `glewInit()` 之前将 `glewExperimental` 全局开关设置为 `GL_TRUE` ，这确保所有具有有效入口点的扩展都会被暴露

------------------

**平台特定扩展**

平台特定扩展被分离到两个头文件中：`wglew.h` 和 `glxew.h`，它们定义了可用的 `WGL` 和 `GLX` 扩展。要确定某个扩展是否被支持，查询 `WGLEW_{extension name}` 或 `GLXEW_{extension_name}`

```cpp
#include <GL/wglew.h>

if (WGLEW_ARB_pbuffer)
{
  // 好的，可以使用 pbuffers
}
else
{
  // 抱歉，pbuffers 在这个平台上不会工作。
}

// 或者，使用 wglewIsSupported 或 glxewIsSupported 从字符串检查扩展：
if (wglewIsSupported("WGL_ARB_pbuffer"))
{
  // 好的，我们可以使用pbuffers。
}
```

------------------

**工具**

GLEW提供了两个命令行工具：一个用于创建可用扩展和视觉效果的列表；另一个用于验证扩展入口点

| 工具 | 作用 | 解释 | 使用 |
| --- | --- | --- | --- |
| `visualinfo` | 扩展和视觉效果 | `visualinfo` 是 `glxinfo` `的扩展版本。Windows` 版本创建了一个名为 `visualinfo.txt` 的文件，其中包含了可用的 `OpenGL`、`WGL` 和 `GLU` 扩展列表以及视觉效果（又称像素格式）的表格。也包括了支持 `Pbuffer` 和 `MRT` 的视觉效果 | `visualinfo -h` |
| `glewinfo` | 扩展验证工具 | `glewinfo` 允许你验证平台上支持的扩展的入口点。`Windows` 版本将结果报告到一个名为 `glewinfo.txt` 的文本文件中。`Unix` 版本将结果打印到 `stdout` | `glewinfo [-pf <id>]` |

那么根据文档所说，我们要在项目中使用 `glewInit()` 函数来初始化 `GLEW`

```cpp
int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	if (GLEW_OK != glewInit()) {
		std::cout << "Error: glewInit Faild" << std::endl;
	}

    // ....
}
```

上面的代码直接运行会出现 `glewInit` 的链接错误

```cpp
GLEWAPI GLenum GLEWAPIENTRY glewInit (void);
```

关注点在上面的 `GLEWAPI` 宏定义中

```cpp
/*
 * GLEW_STATIC is defined for static library.
 * GLEW_BUILD  is defined for building the DLL library.
 */

#ifdef GLEW_STATIC
#  define GLEWAPI extern
#else
#  ifdef GLEW_BUILD
#    define GLEWAPI extern __declspec(dllexport)
#  else
#    define GLEWAPI extern __declspec(dllimport)
#  endif
#endif
```

由于项目中什么宏都没有额外定义，所以会执行 `define GLEWAPI extern __declspec(dllimport)`

根据代码上面的注释，结合项目中使用的是 `glew32s.lib` 静态库，所以需要定义 `GLEW_STATIC` 宏

![](Image/005.png)

在定义宏之后，项目可以正常运行，但是 `glewInit` 返回的却不是 `GLEW_OK`，也就是说 `GLEW` 初始化失败，原因是没有**OpenGL渲染上下文**，这个是文档中说明过的

解决方案很简单，在 `GLFW` 提供的代码 `glfwMakeContextCurrent` 其实已经创建了 OpenGL 上下文，所以只需要在 `glfwMakeContextCurrent` 函数之后 `glewInit` 就行了

```cpp
/* Make the window's context current */
glfwMakeContextCurrent(window);

if (GLEW_OK != glewInit()) {
    std::cout << "Error: glewInit Faild" << std::endl;

}

/* Loop until the user closes the window */
while (!glfwWindowShouldClose(window))
{
    /* Render here */
    glClear(GL_COLOR_BUFFER_BIT);

    render();

    /* Swap front and back buffers */
    glfwSwapBuffers(window);

    /* Poll for and process events */
    glfwPollEvents();
}
```

如果成功，那么可以通过 `std::cout << glGetString(GL_VERSION) << std::endl;` 来输出当前 `OpenGL` 版本

![](Image/006.png)

## 顶点缓冲

### 顶点

顶点缓冲区本质上还是一个**缓冲区**，是一个内存缓冲区，也就是一个数组。也就是定义一组数据来表示三角形，并且将其放到 `GPU` 的 `VRAM` 中，在绘制时 告诉 `GPU` 如何从 `VRAM` 读取并且解释数据信息，以及如何绘制到屏幕上

**顶点** 并不代表坐标，坐标信息只是顶点的一部分，除此之外顶点还可以有其他信息，比如：颜色、法线、纹理等。所以顶点指的是一整个构成顶点的数据集合

由于 OpenGL 是一个很大的状态机，所以要做的就是设置一系列状态和信息，然后告诉 GPU 绘制

```cpp
float positions[6] = {
  -0.5f, -0.5f,
    0.0f,  0.5f,
    0.5f, -0.5f
};
unsigned int buffer;
glGenBuffers(1, &buffer);
glBindBuffer(GL_ARRAY_BUFFER, buffer);
glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW);

glEnableVertexAttribArray(0);
glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);

glBindBuffer(GL_ARRAY_BUFFER, 0);
```

大概介绍一下上述代码的功能

1. `glGenBuffers(1, &buffer)` 创建了一个缓冲区对象。这个缓冲区对象可以用来存储顶点数据、颜色数据等
2. `glBindBuffer(GL_ARRAY_BUFFER, buffer)` 将缓冲区对象绑定到 GL_ARRAY_BUFFER 目标上。这意味着我们将要操作的是一个顶点数组缓冲区
3. 使用 `glBufferData(GL_ARRAY_BUFFER, 6 * sizeof(float), positions, GL_STATIC_DRAW)`，我们将数据从 `positions` 数组传递到缓冲区中
4. 启用了顶点属性数组，使用 `glEnableVertexAttribArray(0)`
5. 使用 `glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0)` 设置了顶点属性指针。这告诉 OpenGL 如何解释缓冲区中的数据
6. 使用 `glBindBuffer(GL_ARRAY_BUFFER, 0)` 解绑了缓冲区对象，确保不再对其进行操作

`glGenBuffers` 函数的作用是为 **OpenGL 缓冲区对象** 分配一个**唯一**的名字（或称为标识符）

在 OpenGL 中，缓冲区对象用于存储图形数据，例如顶点坐标、颜色、法线等

缓冲区对象可以被访问和使用，既可以由应用程序读取，也可以由 GPU 访问

缓冲区对象需要一个唯一的名字来标识它们。这样，OpenGL 可以根据名字找到正确的缓冲区对象

`glGenBuffers` 正是用于生成这些唯一的名字。每次调用 glGenBuffers 都会分配一个新的名字，确保不会与其他缓冲区对象的名字重复

--------------

`glBufferData` 将 `positions` 的数据传递到 `GL_ARRAY_BUFFER` 中，复制了 `sizeof(float) * 6` 个字节的大小数据

| 名称 | 含义 |
| --- | --- |
| STREAM | 这个参数表示数据每帧都不同，即数据会频繁变化。适用于那些每帧都需要更新的缓冲区，例如存储粒子系统的顶点数据 |
| STATIC | 这个参数表示数据不会或几乎不会改变，即一次修改，多次使用。适用于那些在创建后不会频繁修改的缓冲区，例如存储顶点位置、法线等静态数据 |
| DYNAMIC | 这个参数表示数据会被频繁地改变，即多次修改，多次使用。适用于那些需要经常更新的缓冲区，例如存储动态模型的顶点数据 |
| DRAW | 这个参数表示数据将会被送往 GPU 进行绘制。用于指定缓冲区的用途，例如存储顶点数据供渲染使用 |
| READ | 这个参数表示数据会被用户的应用读取。用于指定缓冲区的用途，例如存储纹理数据供 CPU 访问 |
| COPY | 这个参数表示数据会被用于绘制和读取。用于指定缓冲区的用途，例如在数据传输时进行拷贝操作 |

> 这些参数是对缓冲区数据使用模式的提示，帮助 OpenGL 在内部做出更智能的决策，以优化性能

--------------

`glVertexAttribPointer` 则用于告诉 `OpenGL` 如何理解传入的数据。根据 [docs.gl](https://docs.gl/gl4/glVertexAttribPointer) 的解释

```cpp
void glVertexAttribPointer(	GLuint index,
                            GLint size,
                            GLenum type,
                            GLboolean normalized,
                            GLsizei stride,
                            const GLvoid * pointer);
```

- `index`: 起始索引序号，如果数组长度为4，希望从第二个顶点开始绘制，则传入1
- `size`: 表示每个顶点属性的组件数量，如果是一个 Vector3，那么应该设置3
- `type`: 数据类型，比如 `GL_FLOAT` 和 `GL_INT` 等
- `normalized`: 归一化，是否需要将数据归一化到 0~1 的范围内，比如颜色；如果参数是 `GL_FALSE` 则不用归一化
- `stride`: 步长，通过步长计算地址内存偏移，实现数组索引
- `pointer`: 表示顶点属性数据在缓冲区中的起始位置，如果你的数据存储在缓冲区的起始位置，可以将 `pointer` 设置为 0；如果数据存储在缓冲区的其他位置，你需要计算正确的偏移量并设置 `pointer`

![](Image/007.png)

如上图类似，提供一块内存，通过 `index` 知道起始遍历序号，通过 `size` 知道顶点属性的数量，通过 `type` 知道顶点属性的数据类型，通过 `stride` 知道地址需要偏移多少

通过上面传入的数据，就可以完全遍历一块内存了。我们知道区域的数据类型，但是 `OpenGL` 不知道，所以这个函数就是告诉 `OpenGL` 如何解析一块内存区域

### 着色器

前面的代码只是提供了顶点坐标，并没有提供颜色，为什么会绘制出白色的三角形呢？

这个是因为 如果程序没有提供着色器的话， GPU 会驱动提供默认的**着色器**

那么什么是**着色器**？

**渲染管线**（`Render Pipeline`）和**着色器**（`Shaders`）的关系是非常紧密的。在现代图形处理中，渲染管线是由一系列顺序执行的阶段组成的，这些阶段共同完成将3D场景转换为2D图像的任务。着色器则是在这个管线中的关键组件，负责处理图形和图像的具体细节

**渲染管线**是将3D场景转换为2D图像的过程。想象一下它就像一个工厂的流水线，不同的加工环节（渲染阶段）根据用户需求对每个环节进行灵活改造或拆卸，将原始材料（CPU端向GPU端提交的纹理等资源以及指令等）加工为最终的成品，即呈现在用户屏幕上的图像

功能性阶段划分

1. 应用阶段 (Application)
   - 在CPU上执行，完全可控制
   - 主要任务是输入装配，将顶点和索引装配为几何图元
   - 例如，从显存中读取几何数据，装配顶点，构成图元传递给几何阶段
2. 几何阶段 (Geometry Processing)
   - 在GPU上运行，处理应用阶段发送的渲染图元
   - 主要任务是将顶点坐标变换到屏幕空间中，再交给光栅器进行处理
   - 包括顶点着色阶段、投影阶段、裁剪阶段和屏幕映射阶段
3. 光栅化阶段 (Rasterization)
   - 也在GPU上执行
   - 目标是找到处于图元内部的所有像素，将2D坐标顶点转为屏幕上的像素
   - 插值逐像素数据，传递给像素阶段
4. 像素阶段 (Pixel Processing)
   - 处理光栅化阶段传来的像素数据
   - 包括像素着色器，计算像素的颜色和其他属性
   - 最终生成图像

渲染管线的不同阶段会应用不同的着色器

现在GPU允许通过编程的方式，处理顶点、像素的着色操作，通过**着色器**（`Shaders`）的方式

- OpenGL: GLSL语言
- DirectX: HLSL语言

1. 顶点着色器（Vertex Shader）
   - 描述顶点的属性，如位置、纹理坐标、颜色等
   - 将3D空间中的顶点坐标变换为屏幕上的2D坐标
   - 无法生成新的顶点，但输出传递到流水线的下一步
2. 像素着色器（Pixel Shader）
   - 也称为片段着色器，用于计算像素的颜色和其他属性
   - 处理单独的像素，通常指单独的屏幕像素
   - 可以实现光照、凹凸贴图、阴影、半透明等效果
3. 几何着色器（Geometry Shader）
   - 可以生成新的图形基元，如点、线和三角形
   - 可以在图形处理器内修改场景中的几何结构
   - 用于增加模型细节和执行对CPU来说过于繁重的几何操作
4. 曲面细分着色器（Tessellation Shader）
   - 引入于OpenGL 4.0和Direct3D 11
   - 可以将简单网格细分为更复杂的网格，根据特定函数计算
   - 可以根据视点距离等变量动态调整细节层次

![](Image/008.png)

以前面的代码为例子

```cpp
float positions[6] = {
    -0.5f, -0.5f,
    0.0f,  0.5f,
    0.5f, -0.5f
};
```

这里 `positions` 传递了三个顶点，意味着顶点着色器需要运行三次，每个顶点一次

一个顶点着色器的基本目的就是告诉那个顶点需要绘制到屏幕的哪里

然后将数据传递给片段着色器，为每个像素运行一次片段着色器程序，片段着色器的基本目的就是决定像素的颜色

1. 在 OpenGL 中使用 `glCreateShader` 来创建一个 `Shader`
2. 使用 `glShaderSource` 传入指定的 `Shader` 源码
3. 使用 `glCompileShader` 编译指定的 `Shader` 源码

```cpp
static GLuint CompiledShader(const std::string& source, GLenum inType) {
	GLuint id = glCreateShader(inType);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Shader 错误处理
	GLint result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		// alloca 在栈上申请内存，不需要 free ，在作用域结束后自动释放
		char* msg = (char*)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, msg);
		std::cout << "Sharder Compile " << (inType == GL_VERTEX_SHADER ? "vertex sharder" : "fragment sharder") << " Faild" << std::endl;
		std::cout << msg << std::endl;
		
		glDeleteShader(id);
		return GL_ZERO;
	}

	return id;
}
```

一般来说会指定顶点着色器和片段着色器，所以统一封装一个函数用与创建一个程序并将两个着色器绑定到程序中

1. `glCreateProgram` 创建一个程序对象
2. `glAttachShader` 将编译好的着色器附加到程序对象上
3. `glLinkProgram` 链接程序，将所有着色器合并为一个可执行的程序
4. `glValidateProgram` 验证程序对象是否可以在当前的 OpenGL 状态下执行

```cpp
static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader) {
	GLuint program = glCreateProgram();
	GLuint vs = CompiledShader(vertexShader, GL_VERTEX_SHADER);
	GLuint fs = CompiledShader(fragmentShader, GL_FRAGMENT_SHADER);

	glAttachShader(program, vs);
	glAttachShader(program, fs);
	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(fs);
	glDeleteShader(vs);

	return program;
}
```

`glDeleteShader` 表示删除着色器对象，在着色器附加到程序对象并链接之后被调用。这是因为一旦着色器被链接到程序对象，它们就不再需要了，删除它们可以释放资源。着色器的代码已经被链接到程序中，所以可以安全地删除着色器对象

最后只需要调用代码即可创建着色器程序，并通过 `glUseProgram` 来使用它

```cpp
GLuint shader = CreateShaderWithFile("src/Vertex.vert", "src/Fragment.frag");
glUseProgram(shader);
```