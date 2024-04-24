// WindowsProjectTest.cpp : 定义应用程序的入口点。
//

#include "framework.h"
#include "WindowsProjectTest.h"
#include "GTMATH.hpp"
#include "Canvas.h"
#include "Tool.h"
#include "Image.h"

#define MAX_LOADSTRING 100

// 全局变量:
HINSTANCE hInst;                                // 当前实例
WCHAR szTitle[MAX_LOADSTRING];                  // 标题栏文本
WCHAR szWindowClass[MAX_LOADSTRING];            // 主窗口类名

HWND hWnd;                                      // 窗口句柄 经常被用到 所以设置为全局
int wWidth = 800;                               // 窗口宽度
int wHeight = 600;                              // 窗口高度

HDC     hDC;
HDC     hMem;

GT::Canvas* _canvas = nullptr;
GT::Image* _image = nullptr;
GT::Image* _bgImage = nullptr;

// 此代码模块中包含的函数的前向声明:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void Render() {
    _canvas->clear();

    // 测试点
	//for (int x = 0; x < wWidth; ++x) {
	//	_canvas->drawPoint(x, 2 * x, GT::RGBA(255, 0, 0, 0));
	//}

    // 测试直线 极坐标绘制一圈线
    //GT::RGBA Color1 = GT::RGBA(255, 0, 0, 0);
    //GT::RGBA Color2 = GT::RGBA(0, 255, 0, 0);
    //for (int i = 0; i < 360; i += 10) {
    //    float angle = i * PI / 180 ;
    //    int x = cos(angle) * 300 + wWidth / 2;
    //    int y = sin(angle) * 300 + wHeight / 2;
    //    //GT::UTool::DebugPrint(L"angle = %d\n", angle);
    //    _canvas->drawLine(GT::Point(wWidth / 2, wHeight / 2, Color1), GT::Point(x, y, Color2));
    //}

    // 测试三角形
    //_canvas->drawTriangle(
    //    GT::Point(0, wHeight / 2, GT::RGBA(255, 0, 0, 0)), 
    //    GT::Point(wWidth, 0, GT::RGBA(0, 255, 0, 0)), 
    //    GT::Point(wWidth / 2, wHeight, GT::RGBA(0, 0, 255, 0)));

    _canvas->drawImage(100, 100, _bgImage);
    _canvas->drawImage(100, 100, _image);

	// 将 hMem 的数据一次写入到 hDC 中
	BitBlt(hDC, 0, 0, wWidth, wHeight, hMem, 0, 0, SRCCOPY);
}

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

	/*===========创建绘图用的位图========*/
    void* buffer = 0;

    hDC = GetDC(hWnd);  // handle of device context
    hMem = ::CreateCompatibleDC(hDC);
    
	BITMAPINFO  bmpInfo;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biWidth = wWidth;
	bmpInfo.bmiHeader.biHeight = wHeight;
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biCompression = BI_RGB; //实际上存储方式为bgr
	bmpInfo.bmiHeader.biSizeImage = 0;
	bmpInfo.bmiHeader.biXPelsPerMeter = 0;
	bmpInfo.bmiHeader.biYPelsPerMeter = 0;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biClrImportant = 0;

	HBITMAP hBmp = CreateDIBSection(hDC, &bmpInfo, DIB_RGB_COLORS, (void**)&buffer, 0, 0);//在这里创建buffer的内存
	SelectObject(hMem, hBmp);

	memset(buffer, 0, wWidth * wHeight * 4); //清空buffer为0

	/*===========创建绘图用的位图========*/

	_canvas = new GT::Canvas(wWidth, wHeight, buffer);
	_image = GT::Image::readFromFile("res/carma.png");
    _image->setAlpha(.3f);
    _bgImage = GT::Image::readFromFile("res/bk.jpg");
	//_zoomImage = GT::Image::zoomImage(_image, 3, 3);
	//_zoomImageSimple = GT::Image::zoomImageSimple(_image, 3, 3);
	//// _image->setAlpha(0.5);
	//_bkImage = GT::Image::readFromFile("res/bk.jpg");

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

    return (int) msg.wParam;
}



//
//  函数: MyRegisterClass()
//
//  目标: 注册窗口类。
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECTTEST));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
    return RegisterClassExW(&wcex);
}

//
//   函数: InitInstance(HINSTANCE, int)
//
//   目标: 保存实例句柄并创建主窗口
//
//   注释:
//
//        在此函数中，我们在全局变量中保存实例句柄并
//        创建和显示主程序窗口。
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 将实例句柄存储在全局变量中

   hWnd = CreateWindowW(szWindowClass, szTitle, WS_POPUP,
	   100, 100, wWidth, wHeight, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd) 
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  函数: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  目标: 处理主窗口的消息。
//
//  WM_COMMAND  - 处理应用程序菜单
//  WM_PAINT    - 绘制主窗口
//  WM_DESTROY  - 发送退出消息并返回
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 分析菜单选择:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 在此处添加使用 hdc 的任何绘图代码...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// “关于”框的消息处理程序。
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

