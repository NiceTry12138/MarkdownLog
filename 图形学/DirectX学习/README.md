# DirectX学习

[教程地址](https://www.bilibili.com/video/BV1KC4y1Y7tc)

## 创建窗口项目

直接使用 `visual studio` 创建空白项目，并创建 `WinMain.cpp` 作为程序入口

![](Image/001.png)

由于设置的是窗口系统，所以不能使用 `main` 作为入口函数，而是 `WinMain`

```cpp
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance, 
					HINSTANCE hPrevInstance, 
					LPSTR lpCmdLine, 
					int nCmdSHow) {
	while (true)
	{
        // 防止程序结束 用 while 阻塞函数 
	}

	return 0;
}
```

[微软官方文档](https://learn.microsoft.com/zh-cn/windows/win32/)

根据文档解释，`WinMain` 函数的参数分别表示如下几个

- `hInstance` 是 实例的句柄 或模块的句柄。 当可执行文件加载到内存中时，操作系统使用此值来标识可执行文件或 EXE。 某些 Windows 函数需要实例句柄，例如加载图标或位图
- `hPrevInstance` 没有任何意义。 它在 16 位 Windows 中使用，但现在始终为零
- `pCmdLine` 以 Unicode 字符串的形式包含命令行参数
- `nCmdShow` 是一个标志，指示主应用程序窗口是最小化、最大化还是正常显示

> `nCmdShow` 的使用，[文档](https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-showwindow)中也有解释

至于 `WinMain` 函数返回值，一般来说操作系统不使用返回值，但是可以使用该值将状态代码传递给另一个程序

> 一般来说返回 0 表示没有任何问题 

至于 `WINAPI` 和 `CALLBACK` 宏

```cpp
#define CALLBACK    __stdcall
#define WINAPI      __stdcall
#define WINAPIV     __cdecl
```

一般来说

- `WINAPI` 指的是 `Windows API` 使用的一种调用约定。通常用于定义 `API` 函数，很多 `Windows API` 都这样
- `CALLBACK` 常用于回调函数，例如事件处理或窗口处理函数（如 `WindowProc`）。这确保这些函数能与发出回调的 Windows 操作系统兼容

使用 `__stdcall` 调用约定意味着参数从右至左被推送到堆栈上，且函数自己清理堆栈。这对于减少应用程序中的错误非常有用，因为堆栈管理是自动的

除了 `__stdcall` 之外，还有 `__cdecl` 和 `__fastcall`

- `__cdecl`: 参数同样是从右至左推入堆栈，但是调用者清理堆栈。这使得 `__cdecl` 支持可变数量的参数
- `__fastcall`: 一种尽可能通过寄存器而非堆栈传递参数的调用约定，可以提高函数的调用效率，特别是在参数数量较少时

因为 `__stdcall` 是由函数自己清理堆栈，所以需要明确知道堆栈上有多少字节需要被清理。所以 `__stdcall` 不支持可变数量参数。而 `__cdecl` 是调用者清理堆栈，所以根据传递给函数实际参数数量来调整堆栈指针

参数从左到右入栈的顺序不是在常见的 **C/C++** 调用约定中看到的模式，因为在 **C/C++** 中，无论是 `__cdecl` 还是 `__stdcall` 调用约定，参数都是从右到左入栈的。然而，在一些其他语言或特定的场景中，可能会看到从左到右的参数推入顺序。这些语言或平台可能设计了不同的调用约定来满足特定的需求或优化

使用 `WINAPI` 和 `CALLBACK` 宏的主要目的是确保函数与操作系统的互操作性，保持调用约定的一致性，从而使编译生成的代码能够正确地与操作系统交互。不正确的调用约定可能导致运行时错误，比如堆栈损坏，这会是难以调试的错误。通过标准化调用约定，`Windows` 确保了不同编译器和代码库之间的兼容性和稳定性

### 注册窗口

对于一个窗口程序来说，要做的事情有：窗口显示（窗口样式、行为等），在 `Win32` 程序中一般是先**注册**窗口类，再根据注册类创建窗口**实例**，实例就是真正控制的窗口
   
> 一个程序一般不止一个窗口

```cpp
ATOM
WINAPI
RegisterClassExW(
    _In_ CONST WNDCLASSEXW *);
#ifdef UNICODE
#define RegisterClassEx  RegisterClassExW
#else
#define RegisterClassEx  RegisterClassExA
#endif // !UNICODE
```

一般使用 `RegisterClassEx` 来注册类，它是 `RegisterClass` 函数的扩展

- `RegisterClass` 参数为 `WNDCLASS` 的结构指针
- `RegisterClassEx` 参数为 `WNDCLASSEX` 的结构指针，该结构包括 `WNDCLASS` 的全部内容，并添加了额外字段：小图标(`hIconSm`)、任务栏图标等

```cpp
typedef struct tagWNDCLASSEXA {
  UINT      cbSize;             
  UINT      style;              
  WNDPROC   lpfnWndProc;        
  int       cbClsExtra;         
  int       cbWndExtra;
  HINSTANCE hInstance;
  HICON     hIcon;
  HCURSOR   hCursor;
  HBRUSH    hbrBackground;
  LPCSTR    lpszMenuName;
  LPCSTR    lpszClassName;
  HICON     hIconSm;
} WNDCLASSEXA, *PWNDCLASSEXA, *NPWNDCLASSEXA, *LPWNDCLASSEXA;
```

[WNDCLASSEX](https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/ns-winuser-wndclassexa) 官方解释

官方文档对结构体成员属性有比较详细的解释，这里就不再搬运

需要注意的是 `style` 属性，我们使用的是 `CS_OWNDC`，也就是为类中的每个窗口分配唯一的设备上下文，也就是 `Device Context` 简称 `DC`，然后每个窗口就能被独立渲染。通常情况下，多个窗口可能会共享相同的设备上下文。如果一个窗口类被定义为 `CS_OWNDC`，则每个该类的窗口将获取一个独占的设备上下文，并保持这个设备上下文，直到窗口被销毁。这意味着窗口不需要在每次绘制时重新获取设备上下文，可以提高绘制效率

> 关于 `style` 的[官方解释](https://learn.microsoft.com/zh-cn/windows/win32/winmsg/window-class-styles)

另一个需要注意的是 `WNDPROC lpfnWndProc`，指向窗口过程的指针。这个函数将处理所有有关这个窗口的信息，这些消息可以是用户的操作（如键盘输入、鼠标移动、点击等），或者是系统事件（如绘制消息、窗口大小改变等）。

```cpp
typedef LRESULT (CALLBACK* WNDPROC)(HWND, UINT, WPARAM, LPARAM);
```

关于 `WNDPROC` 在[官方文档](https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/nc-winuser-wndproc)有比较详细的解释

### 创建窗口

创建窗口一般使用 `CreateWindowExA` 函数

```cpp
HWND CreateWindowExA(
  [in]           DWORD     dwExStyle,   // 窗口样式
  [in, optional] LPCSTR    lpClassName,
  [in, optional] LPCSTR    lpWindowName,
  [in]           DWORD     dwStyle,     // 窗口样式
  [in]           int       X,           // 窗口位置 X 坐标
  [in]           int       Y,           // 窗口位置 Y 坐标
  [in]           int       nWidth,      // 窗口宽度
  [in]           int       nHeight,     // 窗口高度
  [in, optional] HWND      hWndParent,
  [in, optional] HMENU     hMenu,
  [in, optional] HINSTANCE hInstance,
  [in, optional] LPVOID    lpParam      // 用与传递自定义数据
);
```

关于创建窗口，[官方文档](https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-createwindowexa)提供了比较详细的解释

创建窗口之后需要展示窗口， 也就是 `ShowWindow`

```cpp
BOOL ShowWindow(
  [in] HWND hWnd,
  [in] int  nCmdShow
);
```

关于显示窗口，[官方文档](https://learn.microsoft.com/zh-cn/windows/win32/api/winuser/nf-winuser-showwindow)有比较详细的解释

```cpp
#include <Windows.h>

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdSHow) {

	const wchar_t* pClassName = L"hw3dbutts";

	// 注册类
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(wc);
	wc.style = CS_OWNDC;
	wc.lpfnWndProc = DefWindowProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = nullptr;
	wc.hCursor = nullptr;
	wc.hbrBackground = nullptr;
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = pClassName;
	wc.hIconSm = nullptr;
	RegisterClassEx(&wc);

	// 创建窗口
	HWND hWnd = CreateWindowEx(
		WS_EX_RIGHTSCROLLBAR,
		pClassName,
		L"Hello World",
		WS_SYSMENU | WS_CAPTION | WS_MAXIMIZEBOX,
		200, 200, 640, 480,
		nullptr, nullptr, hInstance, nullptr
	);

	// 展示窗口
	ShowWindow(hWnd, SW_SHOW);


	while (true)
	{

	}

	return 0;
}
```

然后就可以得到一个不能做任何事情的窗口

![](Image/002.png)

### 消息循环

对于窗口来说，除了窗口显示之外，还需要处理信息

比如 `Visual Studio` 需要处理键盘输入，我们要处理的窗口消息(`Window Message`) 本质上来说就是**事件**(`Event`)

当鼠标点击、鼠标移动、键盘输入等事件触发之后，窗口会首先把消息按顺序放进到消息队列(`Message Queue`) 中，可以通过 `GetMessage` 来获取队列中的消息，之后通过 `DispatchMessage` 把消息从应用传递给对应的窗口的 `lpfnWndProc` 函数

![](Image/003.png)

```cpp
BOOL GetMessage(
  [out]          LPMSG lpMsg,           // 消息的指针
  [in, optional] HWND  hWnd,            // 处理信息的窗口指针
  [in]           UINT  wMsgFilterMin,
  [in]           UINT  wMsgFilterMax
);
```

`wMsgFilterMin` 和 `wMsgFilterMax` 用与过滤信息

如果 `hWnd` 为 `NULL`，`GetMessage` 将检索属于当前线程的任何窗口的消息

`GetMessage` 的值也同样需要注意，如果是退出窗口 `WM_QUIT` 则返回值为 0，否则是非零值。如果出现错误，则返回 -1

```cpp
LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	default:
		break;
	}
	return DefWindowProc(hWnd, msg, wParam, lParam);
}

// WinMain function 
{
    // Do Something 

	// 消息处理
	MSG msg;
	BOOL gResult;

	while ((gResult = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (gResult < 0) {
		return -1;	// 表示程序错误
	}
	return msg.wParam;	// 否则输出我们期望的值 也就是 PostQuitMessage 传入的参数值
}
```

这里使用 `WndProc` 自定义的函数来接管默认的 `DefWindowProc`(`Def` 开头表示 `Default`)，然后特殊处理 `WM_CLOSE` 时关闭程序，否则 `DefWindowProc` 只会关闭窗口而不会关闭进程

`PostQuitMessage` 函数将 `WM_QUIT` 消息发布到线程的消息队列并立即返回;函数只是向系统指示线程正在请求在将来的某个时间退出

当线程从其消息队列中检索 `WM_QUIT` 消息时，它应退出其消息循环，并将控制权返回到系统。 返回到系统的退出值必须是 `WM_QUIT` 消息的 `wParam` 参数

所以最后 `WinMain` 的输出是 `msg.wParam`，这样程序的代码是可以自定义的，未来可以通过这个结束码通知其他进程

![](Image/004.png)

> 输出为 69 结果生效

这里使用 `PostQuitMessage(69)` 没有任何含义，单纯就是为了测试输出结果是否生效


消息循环的类型有很多

[list of windows Message](https://wiki.winehq.org/List_Of_Windows_Messages)

大概四百种类型，每种消息的触发条件可能需要自行测试

当然官网上也有一些消息类型的解释

除了官网和谷歌之外，还可以通过运行代码测试，何种情况触发何种宏来确定宏的触发条件

项目中使用 `WIndowsMessageMap` 来测试宏的触发，[代码地址](https://github.com/planetchili/hw3d/blob/master/hw3d/WindowsMessageMap.cpp)

以键盘按键为例

![](Image/005.png)

一次键盘的按下和松开会触发三个消息：`WM_KEYDOWN`、`WM_CHAR` 和 `WM_KEYUP`。当按下 `D` 键时，`WM_KEYDOWN` 的 `wParam` 输出为 **0x0000044** ；当按下 `F` 键时，`wParam` 输出为 **0x0000046**，所以 `wParam` 可能存储了按下按钮相关信息

- 关于 `WM_CHAR` 具体内容可以查[官方文档](https://learn.microsoft.com/zh-cn/windows/win32/inputdev/wm-char)
- 关于 `lParam` 表示的虚拟按键，可以通过[官方文档](https://learn.microsoft.com/zh-cn/windows/win32/inputdev/virtual-key-codes) 获取更多信息

以鼠标点击为例

![](Image/006.png)

主要的消息触发就是：`WM_LBUTTONDOWN` 和 `WM_LBUTOTNUP` 来表示鼠标左键的点击和松开，对应的鼠标右键点击就是 `WM_RBUTTONDOWN` 和 `WM_RBUTTONUP`，鼠标移动有 `WM_MOUSEMOVE`

 