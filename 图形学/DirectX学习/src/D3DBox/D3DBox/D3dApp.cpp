#include "D3dApp.h"
#include <Windows.h>

using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    // 转发窗口消息
	return D3DApp::GetApp()->MsgProc(hwnd, msg, wParam, lParam);
}

D3DApp* D3DApp::mApp = nullptr;

D3DApp::D3DApp(HINSTANCE hInstance)
{
	mhAppInst = hInstance;

    // 单例模式 禁止各种方式创建第二个
	assert(mApp == nullptr);
    mApp = this;
}

D3DApp::~D3DApp()
{
    if (md3dDevice != nullptr) {
        FlushCommandQueue();    // 清空 GPU 命令
    }
}

D3DApp* D3DApp::GetApp()
{
	return mApp;
}

HINSTANCE D3DApp::AppInst() const
{
    return mhAppInst;
}

HWND D3DApp::MainWnd() const
{
    return mhMainWnd;
}

float D3DApp::AspectRatio() const
{
    return mClientWidth * 1.0f / mClientHeight;
}

bool D3DApp::Get4xMassState()
{
    return m4xMsaaState;
}

void D3DApp::Set4XMassState(bool Value)
{
	if (m4xMsaaState != Value)
	{
		m4xMsaaState = Value;

		CreateSwapChain();
		OnResize();
	}
}

int D3DApp::Run()
{
	MSG msg{ 0 };
	mTimer.Reset();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else {
			mTimer.Tick();
			if (!mAppPaused) {
				CalculateFrameState();
				Update(mTimer);
				Draw(mTimer);
			}
			else {
				Sleep(100);
			}
		}
	}

    return 0;
}

bool D3DApp::Initialize()
{
    if (!InitMainWindow()) {
        return false;
    }

    if (!InitDirect3D()) {
        return false;
    }

    OnResize();

    return true;
}

LRESULT D3DApp::MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	default:
		break;
	}
    return DefWindowProc(hWnd, msg, wParam, lParam);
}

void D3DApp::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 1;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}

void D3DApp::OnResize()
{
	// 检查资源是否初始化完毕
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mCommandAllocator);

	// 清空命令队列
	FlushCommandQueue();
	mCommandList->Reset(mCommandAllocator.Get(), nullptr);

	// 释放资源 重新创建
	for (int i = 0; i < SwapChainBufferCount; ++i) {
		mSwapChainBuffer[i].Reset();
	}
	mDepthStencilBuffer.Reset();
	
	// 重新设置交换链大小
	mSwapChain->ResizeBuffers(SwapChainBufferCount, mClientWidth, mClientHeight, mBackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH);

	// 创建 RTV
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (int i = 0; i < SwapChainBufferCount; ++i) {
		mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i]));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHandle);
		rtvHandle.Offset(1, mRtvDescriptorSize);
	}

	// 创建 BSV
	// 资源的维度、大小、格式等属性。在这里，它被用来定义深度模板缓冲区的特性，如宽度、高度、格式等
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.Alignment = 0;				// 指定资源对齐方式 一般设置为0 即默认对齐 一般是 64kb
	depthStencilDesc.DepthOrArraySize = 1;		// 指定资源的深度(3D)或者数组大小(2D、1D)，设置为1表示单层资源 不是资源数组
	depthStencilDesc.MipLevels = 1;				// MIP 用于不同距离显示不同级别的细节，设置为1表示没有额外的 Mip 级别
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;


	// 定义资源的清除值。对于深度模板资源，这通常包括一个深度值和一个模板值。这个值在资源创建时被用来初始化资源的内容，以便于后续的清除操作
	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0;
	optClear.DepthStencil.Stencil = 0;

	// 创建深度模板缓冲区，并将其初始化为常见状态，同时使用 optClear 作为优化的清除值
	md3dDevice->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT), D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf()));

	// 如何从深度模板视图访问纹理的子资源。它定义了视图的格式和维度，以及如何解释深度模板数据
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	// 创建一个深度模板视图，用于访问深度模板资源的数据
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	// 更新资源状态为可写入
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	
	mCommandList->Close();
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };

	// 执行命令
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	FlushCommandQueue();

	// 更新视口和裁剪区域
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, mClientWidth, mClientHeight };
}

// 基本流程： 注册、创建、展示、更新
bool D3DApp::InitMainWindow()
{
	WNDCLASS wc;
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = MainWndProc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = mhAppInst;
	wc.hIcon = LoadIcon(0, IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszMenuName = 0;
	wc.lpszClassName = L"MainWnd";

	if (!RegisterClass(&wc))
	{
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	RECT R = { 0, 0, mClientWidth, mClientHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	mhMainWnd = CreateWindow(L"MainWnd", mMainWndCaption.c_str(), 
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width, height, 0, 0, mhAppInst, 0);

	if (!mhMainWnd) {
		MessageBox(0, L"RegisterClass Failed.", 0, 0);
		return false;
	}

	ShowWindow(mhMainWnd, SW_SHOW);
	UpdateWindow(mhMainWnd);	// 迫使系统立即处理那些已经在消息队列中的绘制消息，确保窗口首次创建之后，会被立即正确绘制

    return true;
}

bool D3DApp::InitDirect3D()
{
#if defined(DEBUG) || defined(_DEBUG)
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
#endif

	// 创建 DXGI 
	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	// 获得光栅器
	HRESULT hardwareResult = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&md3dDevice));
	if (FAILED(hardwareResult)) {
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));
		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&md3dDevice)));
	}

	// 创建围栏
	ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

	// 存储描述符大小
	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// 设置 4x 多重采样 
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(md3dDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)));

	m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

#ifdef _DEBUG
	LogAdapters();
#endif // _DEBUG


	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();

    return true;
}

void D3DApp::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	ThrowIfFailed(md3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&mCommandAllocator)));

	ThrowIfFailed(md3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(&mCommandList)));

	mCommandList->Close();
}

void D3DApp::CreateSwapChain()
{
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;
	sd.OutputWindow = mhMainWnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	ThrowIfFailed(mdxgiFactory->CreateSwapChain(mCommandQueue.Get(), &sd,mSwapChain.GetAddressOf()));
}

void D3DApp::FlushCommandQueue()
{
	mCurrentFence++;

	// 发送一个信号到命令队列。当GPU处理到这一点时，它会将围栏对象 mFence 的值设置为 mCurrentFence。这样可以确保所有先前提交的命令都已经被GPU处理完毕
	mCommandQueue->Signal(mFence.Get(), mCurrentFence);

	if (mFence->GetCompletedValue() < mCurrentFence) {
		// 创建一个事件对象，这个事件用于等待围栏信号的完成
		HANDLE eventHandle = CreateEventEx(nullptr, NULL, false, EVENT_ALL_ACCESS);

		// 为围栏设置一个完成事件。当围栏达到指定的 mCurrentFence 值时，eventHandl 事件会被设置为信号状态
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

		// 等待事件对象变为信号状态
		WaitForSingleObject(eventHandle, INFINITE);

		// 关闭事件句柄，释放系统资源
		CloseHandle(eventHandle);
	}
}

ID3D12Resource* D3DApp::CurrentBackBuffer() const
{
	return mSwapChainBuffer[mCurrentBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrentBackBuffer,
		mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE D3DApp::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

void D3DApp::CalculateFrameState()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		wstring fpsStr = to_wstring(fps);
		wstring mspfStr = to_wstring(mspf);

		wstring windowText = mMainWndCaption +
			L"    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText(mhMainWnd, windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void D3DApp::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"*** Adapter";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());
		adapterList.push_back(adapter);

		++i;
	}

	for (size_t index = 0; index < adapterList.size(); ++index)
	{
		LogAdapterOutputs(adapterList[index]);
		ReleaseCom(adapterList[index]);
	}
}

void D3DApp::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayMode(output, mBackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void D3DApp::LogOutputDisplayMode(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}
