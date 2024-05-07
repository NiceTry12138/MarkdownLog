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

![](Image/003.png)

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