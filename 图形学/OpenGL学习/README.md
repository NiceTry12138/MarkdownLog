# OpenGL

## 创建窗口

使用 `glfw` 来学习 `OpenGL` 的 API， 网址 [www.flgw.org](https://www.glfw.org)，以及[示例代码](https://www.glfw.org/documentation.html)

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

