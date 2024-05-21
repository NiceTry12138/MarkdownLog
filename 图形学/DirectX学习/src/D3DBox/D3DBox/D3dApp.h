#pragma once

#if defined(DEBUG) || defined(_DEBUG)
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include "d3dUtil.h"
//#include "GameTimer.h"

// Link necessary d3d12 libraries.
#pragma comment(lib,"d3dcompiler.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxgi.lib")

class GameTimer;

class D3DApp
{
protected:
	D3DApp(HINSTANCE hInstance);
	D3DApp(const D3DApp& rhs) = delete;
	D3DApp& operator=(const D3DApp& rhs) = delete;
	virtual ~D3DApp();

public:
	static D3DApp* GetApp();

	HINSTANCE	AppInst() const;
	HWND		MainWnd() const;
	float		AspectRatio() const;			// 纵横比

	bool		Get4xMassState();				// 是否开启 4xMass 多重采样
	void		Set4XMassState(bool Value);		// 设置多重采样的开关

	int			Run();							// 初始化完毕之后执行

	virtual bool Initialize();					// 初始化窗口和D3D
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);		// 窗口消息触发函数

protected:
	virtual void CreateRtvAndDsvDescriptorHeaps();	// 创建 RTV 和 DSV 的堆描述符
	virtual void OnResize();						// 窗口重新设置大小是调用
	virtual void Update(const GameTimer& gt) = 0;	// 每帧更新逻辑
	virtual void Draw(const GameTimer& gt) = 0;		// 每帧绘制

	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}	// 鼠标点击事件
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}	// 鼠标松开事件
	virtual void OnMouseMove(WPARAM btnStae, int x, int y) {}	// 鼠标移动事件

protected:

	bool InitMainWindow();							// 初始化窗口
	bool InitDirect3D();							// 初始化 D3D
	
	void CreateCommandObjects();					// 创建命令队列、分配器、列表
	void CreateSwapChain();							// 创建交换链
	
	void FlushCommandQueue();						// 等待清空命令列表命令

	ID3D12Resource* CurrentBackBuffer() const;		// 当前交换链中的后台缓冲区
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;	// 当前交换链中后台缓冲区视图
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;		// 当前深度模板视图

	void CalculateFrameState();						// 计算当前帧状态
	
	void LogAdapters();								// 输出所有适配器
	void LogAdapterOutputs(IDXGIAdapter* adapter);	// 输出指定适配器信息
	void LogOutputDisplayMode(IDXGIOutput* output, DXGI_FORMAT format);	// 根据指定颜色格式输出显卡信息

protected:
	static D3DApp* mApp;

	HINSTANCE	mhAppInst = nullptr;
	HWND		mhMainWnd = nullptr;
	bool		mAppPaused = false;		// 当前程序是否被暂停
	bool		mMinimized = false;		// 当前程序是否最小化
	bool		mMaximized = false;		// 当前程序是否最大化
	bool		mResizing = false;		// 当前程序正在重新设置大小
	bool		mFullscreenState = false;	// 当前程序是否全屏状态

	bool		m4xMsaaState = false;	// 是否开启4x多重采样
	UINT		m4xMsaaQuality = 0;		// 多重采样质量

	//GameTimer	mTimer;					// 时间计时器

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;	// DXGI 工厂
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;	// 交换链
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;	// 设备

	Microsoft::WRL::ComPtr<ID3D12Fence>	mFence;			// 围栏
	UINT64 mCurrentFence = 0;							// 围栏序号


	Microsoft::WRL::ComPtr<ID3D12CommandQueue>	mCommandQueue;	// 命令队列
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;	// 命令内存分配器
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	mCommandList;	// 命令列表

	static const int SwapChainBufferCount = 2;			// 渲染缓冲个数 这里表示使用双缓冲
	int mCurrentBackBuffer = 0;							// 当前后台缓冲的序号
	
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];	// 双缓冲
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;		// 深度缓冲

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;	// Render Target View 描述符堆
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;	// Depth Stencil View 描述符堆

	D3D12_VIEWPORT mScreenViewport;							// 视口，定义了渲染到屏幕的区域
	D3D12_RECT mScissorRect;								// 剪裁矩形，定义了在视口内可见的矩形区域

	UINT mRtvDescriptorSize = 0;							// Rtv 描述符大小 不同平台有所不同
	UINT mDsvDescriptorSize = 0;							// Dsv 描述符大小
	UINT mCbvSrvUavDescriptorSize = 0;						// Cbv Srv Uav 描述符大小

	std::wstring mMainWndCaption = L"d3d App";							// 主窗口标题
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;			// Direct3D 驱动类型，通常是硬件加速
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;			// 后台缓冲区的像素格式
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;	// 深度模板缓冲区的格式
	
	int mClientWidth = 800;
	int mClientHeight = 600;
};

