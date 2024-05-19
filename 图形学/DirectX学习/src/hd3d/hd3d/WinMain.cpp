//#include <Windows.h>
#include "ChiliWin.h"
#include <cstdlib>
#include <sstream>
#include "Window.h"
#include "Test/WindowsMessageMap.h"

inline std::wstring to_wide_string(const std::string& input) //string to wstring
{
	std::wstring wide(input.size(), L'\0');
	mbstowcs(&wide[0], input.c_str(), input.size());
	return wide;
}

//LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
//	static WindowsMessageMap mm;
//
//	OutputDebugString(to_wide_string(mm(msg, lParam, wParam)).c_str());
//
//	switch (msg)
//	{
//	case WM_CLOSE:
//		PostQuitMessage(69);
//		break;
//	case WM_KEYDOWN:
//		if (wParam == 'F') {
//			// 当 F 键按下，修改窗口的 Title
//			SetWindowText(hWnd, L"Reset Title");
//		}
//		break;
//	case WM_CHAR:
//	{
//		static std::string title;
//		title.push_back((char)wParam);
//		SetWindowText(hWnd, to_wide_string(title).c_str());
//		break;
//	}
//	case WM_LBUTTONDOWN:
//	{
//		POINTS pt = MAKEPOINTS(lParam);
//		std::ostringstream oss;
//		oss << "(" << pt.x << ", " << pt.y << ")";
//		SetWindowText(hWnd, to_wide_string(oss.str()).c_str());
//		break;
//	}
//	default:
//		break;
//	}
//	return DefWindowProc(hWnd, msg, wParam, lParam);
//}

int WINAPI WinMain(HINSTANCE hInstance,
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nCmdSHow) {


	Window wnd(800, 300, L"Donkey Fart Box");
	//Window wnd2(300, 800, L"Donkey Fart Box");	// 直接通过构造函数创建第二个窗口

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