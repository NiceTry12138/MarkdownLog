# OpenGL

## 光栅化理论及实现

### 创建窗口

`visual studio` 创建 **Windows 桌面应用程序**

```cpp
// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
```

| 函数名 | 作用 |
| --- | --- |
| MyRegisterClass | 这个函数用于注册窗口类。窗口类必须在创建窗口之前注册。这个函数通常会设置窗口的样式、图标、光标以及窗口过程等 |
| InitInstance | 这个函数用于创建和显示窗口。它通常接受一个应用程序实例句柄和一个命令显示标志（如 SW_SHOW），并返回一个布尔值，指示窗口是否成功创建 |
| WndProc | 这是窗口过程函数，是一个消息处理函数，用于响应发送到窗口的各种消息，如按键、鼠标事件、绘制请求等 |
| About | 这通常是一个对话框过程，用于处理“关于”对话框的消息。这个函数类似于窗口过程，但专门用于对话框 |

为了提供一个结构化和模块化的方式来管理 Windows 应用程序的不同部分。通过将全局变量和函数声明放在一个地方，可以更容易地在整个程序中访问和管理它们。此外，前向声明允许在实际定义函数之前引用它们，这有助于解决头文件和源文件之间的依赖关系问题

```cpp
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: 在此处放置代码。

    // 初始化全局字符串
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECTTEST, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // 执行应用程序初始化:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECTTEST));

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}
```

这就是 **Windows窗体程序** 的入口函数

- `_In_ HINSTANCE hInstance`：当前实例的句柄
- `_In_opt_ HINSTANCE hPrevInstance`：以前实例的句柄，现在总是为 NULL
- `_In_ LPWSTR lpCmdLine`：命令行参数的字符串
- `_In_ int nCmdShow`：控制窗口如何显示的标志

```cpp
UNREFERENCED_PARAMETER(hPrevInstance);
UNREFERENCED_PARAMETER(lpCmdLine);
```

这两行代码是宏，用于避免编译器警告，因为 `hPrevInstance` 和 `lpCmdLine` 参数未被使用

```cpp
LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
LoadStringW(hInstance, IDC_WINDOWSPROJECTTEST, szWindowClass, MAX_LOADSTRING);
```

这两行代码使用 `LoadStringW` 函数从资源文件加载字符串到 `szTitle` 和 `szWindowClass` 变量中

`LoadStringW` 函数用于从与指定模块关联的可执行文件中加载字符串资源。这里的 `hInstance` 句柄是必需的，因为它指定了包含所需字符串资源的模块的实例。在 `Windows` 编程中，每个运行的程序或动态链接库（DLL）都有一个 `HINSTANCE`，这是一个唯一的标识符，用于区分不同的程序和资源

在项目中存在一个后缀为 `rc` 的文件，其文件名一般是 `项目名称.rc`，作用是配置表

![](Image/001.png)

如上图所示，宏 `IDS_APP_TITLE` 和 `IDC_WINDOWSPROJECTTEST` 的值对应的字符串内容已经配置在表中

```cpp
MyRegisterClass(hInstance);
```

调用 MyRegisterClass 函数来注册窗口类，这是创建窗口所必需的

```cpp
if (!InitInstance (hInstance, nCmdShow))
{
    return FALSE;
}
```

调用 InitInstance 函数来创建和显示窗口。如果窗口创建失败，则函数返回 FALSE

```cpp
while (GetMessage(&msg, nullptr, 0, 0))
```

这是主消息循环。GetMessage 函数从消息队列中检索消息。如果获取到的消息不是退出消息（WM_QUIT），则循环继续

```cpp
if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
{
    TranslateMessage(&msg);
    DispatchMessage(&msg);
}
```

在消息循环中，首先检查是否有加速键消息。如果没有，则使用 TranslateMessage 函数翻译消息（如键盘输入），然后使用 DispatchMessage 函数将消息分发给窗口过程

至于 `WndProc` ，当一个窗口创建后，操作系统会将各种消息发送到这个窗口，如按键、鼠标移动、绘制请求等。每当这些消息发生时，操作系统会调用与窗口关联的 `WndProc` `函数来处理这些消息。WndProc` 函数的参数包括一个窗口句柄（`HWND`），一个消息代码（`UINT`），以及两个消息特定的参数（`WPARAM` 和 `LPARAM`）

在消息循环中，`GetMessage` 或 `PeekMessage` 函数从消息队列中检索消息，并将其传递给 `TranslateMessage` 和 `DispatchMessage` `函数。DispatchMessage` 函数随后会根据消息中包含的窗口句柄找到相应的窗口过程并调用它

例如，如果用户点击了窗口的关闭按钮，操作系统会发送一个 `WM_CLOSE` 消息到消息队列。`GetMessage` 会从队列中获取这个消息，然后 `DispatchMessage` 会调用窗口的 `WndProc` 函数，并将 `WM_CLOSE` 作为消息代码传递进去，由 `WndProc` 函数来决定如何响应这个消息

总的来说，`WndProc` 函数是应用程序与操作系统之间通信的桥梁，它负责处理所有针对窗口的消息和事件

1. 为了方便对窗口进行处理，设置 `HWND hWnd;` 的全局变量
2. 为了方便计算，设置窗体的宽高 `int wWidth = 800;int wHeight = 600;`
3. 为了方便去除按钮对窗体大小的影响，设置类型为 `WS_POPUP`

```cpp
HWND hWnd;                                      // 窗口句柄 经常被用到 所以设置为全局
int wWidth = 800;                               // 窗口宽度
int wHeight = 600;                              // 窗口高度

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    // Other
    hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP,
      CW_USEDEFAULT, 0, wWidth, wHeight, nullptr, nullptr, hInstance, nullptr);

    // Other
   return TRUE;
}
```

| 窗口样式 | 值 |
| --- | --- |
| WS_BORDER | 窗口有一个薄边框 |
| WS_CAPTION | 窗口有标题栏（包括 WS_BORDER 样式） |
| WS_CHILD | 窗口是一个子窗口。具有此样式的窗口不能有菜单栏，并且不能与 WS_POPUP 样式一起使用 |
| WS_DISABLED | 窗口最初被禁用。禁用的窗口无法接收用户输入 |
| WS_DLGFRAME | 窗口有一个通常用于对话框的边框样式 |
| WS_HSCROLL | 窗口有水平滚动条 |
| WS_VSCROLL | 窗口有垂直滚动条 |
| WS_SYSMENU | 窗口有系统菜单 |
| WS_THICKFRAME | 窗口有一个可调整大小的边框 |
| WS_GROUP | 窗口是一组控件的第一个控件 |
| WS_TABSTOP | 窗口是一个制表位，用户可以使用 TAB 键在控件之间导航 |
| WS_MINIMIZE | 窗口最初被最小化 |
| WS_MAXIMIZE | 窗口最初被最大化 |
| WS_OVERLAPPEDWINDOW | 这是一个窗口样式的组合，它创建一个具有标题栏、大小调整边框、窗口菜单以及最小化和最大化按钮的重叠窗口1。这种样式通常用于应用程序的主窗口 |
| WS_POPUP | 创建一个弹出窗口。弹出窗口是顶级窗口，它们与桌面窗口的子窗口列表相连接。应用程序通常使用弹出窗口来显示对话框。弹出窗口与重叠窗口的主要区别在于，弹出窗口不需要有标题，而重叠窗口必须有标题。当弹出窗口没有标题时，它可以创建没有边框的 |

> 还有很多其他类型

### 设置画布

```cpp
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    // 注册、初始化、创建窗口
    // .....

    /*===========创建绘图用的位图========*/
    void* buffer = 0;

    hDC = GetDC(hWnd);                      // 获取窗口 hWnd 的设备上下文（DC），用于绘图操作
    hMem = ::CreateCompatibleDC(hDC);       // 创建一个与指定设备上下文（hDC）兼容的内存设备上下文（Memory DC）。这个内存 DC 用于离屏绘制，即在屏幕外部绘制图像

    BITMAPINFO  bmpInfo;                    // 它描述了位图的维度和颜色格式
    bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfo.bmiHeader.biWidth = wWidth;
    bmpInfo.bmiHeader.biHeight = wHeight;
    bmpInfo.bmiHeader.biPlanes = 1;             // 平面数
    bmpInfo.bmiHeader.biBitCount = 32;          // 每像素位数（32位，即每个像素4字节）
    bmpInfo.bmiHeader.biCompression = BI_RGB;   // 压缩方式，实际上存储方式为bgr
    bmpInfo.bmiHeader.biSizeImage = 0;
    bmpInfo.bmiHeader.biXPelsPerMeter = 0;
    bmpInfo.bmiHeader.biYPelsPerMeter = 0;
    bmpInfo.bmiHeader.biClrUsed = 0;
    bmpInfo.bmiHeader.biClrImportant = 0;

    HBITMAP hBmp = CreateDIBSection(hDC, &bmpInfo, DIB_RGB_COLORS, (void**)&buffer, 0, 0); //在这里创建buffer的内存，创建一个设备无关位图，并将其与一个内存块关联。这个内存块由 buffer 指向，用于存储位图的像素数据
    SelectObject(hMem, hBmp);   // 将新创建的位图（hBmp）选入之前创建的内存设备上下文（hMem）中，这样在 hMem 上的绘图操作将影响到这个位图

    memset(buffer, 0, wWidth * wHeight * 4); //清空buffer为0

    MSG msg;

    // 主消息循环:
    while (true)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        BitBlt(hDC, 0, 0, wWidth, wHeight, hMem, 0, 0, SRCCOPY);
    }

    return (int) msg.wParam;
}
```

这段代码创建了一个可以用于离屏绘制的位图，并准备了相应的内存设备上下文和像素数据缓冲区。这样，您可以在位图上进行绘图操作，然后将其绘制到窗口上，或者进行其他图像处理操作

| 变量名 | 作用 |
| --- | --- |
| hDC（Device Context Handle） |  hDC 是一个设备上下文句柄，它代表了一个绘图表面的整个状态。您可以在这个设备上下文上进行绘图操作，如画线、画形状、输出文本等。在这个例子中，hDC 是通过 GetDC 函数从一个窗口获取的，因此它代表了窗口的客户区的绘图表面 |
| hMem（Memory Device Context Handle） | hMem 是一个内存设备上下文句柄，它是一个与 hDC 兼容的内存中的绘图表面。您可以在这个内存设备上下文上进行绘图操作，而不会影响实际的屏幕。这种技术通常用于复杂的绘图操作，因为它可以减少屏幕闪烁并提高绘图效率 |
| hBmp（Bitmap Handle） | hBmp 是一个位图句柄，它代表了一个设备无关位图（DIB）。这个位图是通过 CreateDIBSection 函数创建的，它可以直接访问像素数据，并且可以被选入一个设备上下文中进行绘图。
buffer（Pointer to the Bitmap’s Pixel Data） |  buffer 是一个指针，它指向 CreateDIBSection 函数创建的位图的像素数据。这个数据区域可以被直接访问和修改，以改变位图的内容 |

- `hDC` 用于获取窗口的设备上下文，它是绘图操作的起点
- `hMem` 是从 hDC 创建的，用于在内存中进行绘图操作，这样的操作不会立即反映在用户的屏幕上
- `hBmp` 是一个位图，它被创建并与 `hMem` 关联，这样在 `hMem` 上的所有绘图操作都会影响到这个位图
- `buffer` 是 `hBmp` 的像素数据的直接访问点，通过修改 `buffer`，可以改变位图的内容

总结一下就是，`hDC` 是屏幕的绘图表面，`hMem` 是内存中的绘图表面，`hBmp` 是内存中的位图，`buffer` 是位图的像素数据。在 `hMem` 和 `buffer` 上的修改最终会通过 `hDC` 显示在屏幕上

当完成了所有的绘图操作后，通常会使用 `BitBlt` 或 `StretchBlt` 等函数将 `hMem` 中的位图内容传输到 `hDC`，从而将图像显示在屏幕上。这个过程称为 **双缓冲**（`double buffering`）

如果直接修改 `hDC`，会导致图形界面更新，用户可能会看到部分重绘过程，导致闪烁。因此使用双缓冲，`hDC` 只用于给显示器读取数据用于显示，`hMem` 用于计算，再计算完毕之后一次性写入到 `hDC` 中

`BITMAPINFO` 是产生位图所需的信息，主要包含两个部分

1. `BITMAPINFOHEADER`：这是一个结构体，包含了位图的基本信息，如位图的大小、宽度、高度、颜色平面数、每像素位数、压缩类型、图像大小等
2. `bmiColors`：这是一个颜色表，可以是 `RGBQUAD` 数组，也可以是指定颜色表中颜色的索引。颜色表的具体内容取决于 `BITMAPINFOHEADER` 中的 `biBitCount` 和 `biClrUsed` 成员的值

**位图**，也称为栅格图像或点阵图像，是由像素（图片元素）的单个点组成的图像。每个像素都有自己的颜色信息，位图通常用于存储数字照片和其他类型的图像。位图的特点是可以精确地控制每个像素，但缺点是放大后会出现像素化，且文件大小通常比矢量图像

### 点的绘制

```cpp
class Canvas
{
private:
    int m_Width{ -1 };
    int m_Height{ -1 };
    RGBA* m_Buffer{ nullptr };


public: 
    Canvas(int _width, int _height, void* _buffer) {
        if (_width <= 0 || _height <= 0) {
            m_Width = -1;
            m_Height = -1;
        }
        m_Width = _width;
        m_Height = _height;
        m_Buffer = (RGBA*)_buffer;
    }

    ~Canvas() {

    }

    // 画点操作
    void drawPoint(int x, int y, RGBA _collor) {
        if (x < 0 || x >= m_Width || y < 0 || y >= m_Height) {
            return;
        }

        m_Buffer[y * m_Height + x] = _collor;
    }

    // 清理操作
    void clear() {
        if (m_Buffer != nullptr) {
            memset(m_Buffer, 0, sizeof(RGBA) * m_Width * m_Height);
        }
    }
};
```

封装 Canvas 类，用于绘制点以及对外提供接口

在 `wWinMain` 中创建并初始化全局 `Canvas` 对象

```cpp
GT::Canvas* _canvas = nullptr;

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{

	/*===========创建绘图用的位图========*/

	_canvas = new GT::Canvas(wWidth, wHeight, buffer);omFile("res/bk.jpg");

    MSG msg;

    // 主消息循环:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        Render();
    }
}
```

随后在 `Render` 的时候设置绘制点的信息

```cpp
void Render() {
    _canvas->clear();

    for (int x = 0; x < wWidth; ++x) {
        _canvas->drawPoint(x, 200, GT::RGBA(255, 0, 0, 0));
    }

	// 将 hMem 的数据一次写入到 hDC 中
	BitBlt(hDC, 0, 0, wWidth, wHeight, hMem, 0, 0, SRCCOPY);
}
```

上述代码在高度为 100 的地方绘制了一条横着的红线


## 图形处理及纹理系统



## 图形学状态机接口封装


