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
	float		AspectRatio() const;			// �ݺ��

	bool		Get4xMassState();				// �Ƿ��� 4xMass ���ز���
	void		Set4XMassState(bool Value);		// ���ö��ز����Ŀ���

	int			Run();							// ��ʼ�����֮��ִ��

	virtual bool Initialize();					// ��ʼ�����ں�D3D
	virtual LRESULT MsgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);		// ������Ϣ��������

protected:
	virtual void CreateRtvAndDsvDescriptorHeaps();	// ���� RTV �� DSV �Ķ�������
	virtual void OnResize();						// �����������ô�С�ǵ���
	virtual void Update(const GameTimer& gt) = 0;	// ÿ֡�����߼�
	virtual void Draw(const GameTimer& gt) = 0;		// ÿ֡����

	virtual void OnMouseDown(WPARAM btnState, int x, int y) {}	// ������¼�
	virtual void OnMouseUp(WPARAM btnState, int x, int y) {}	// ����ɿ��¼�
	virtual void OnMouseMove(WPARAM btnStae, int x, int y) {}	// ����ƶ��¼�

protected:

	bool InitMainWindow();							// ��ʼ������
	bool InitDirect3D();							// ��ʼ�� D3D
	
	void CreateCommandObjects();					// ����������С����������б�
	void CreateSwapChain();							// ����������
	
	void FlushCommandQueue();						// �ȴ���������б�����

	ID3D12Resource* CurrentBackBuffer() const;		// ��ǰ�������еĺ�̨������
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView() const;	// ��ǰ�������к�̨��������ͼ
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView() const;		// ��ǰ���ģ����ͼ

	void CalculateFrameState();						// ���㵱ǰ֡״̬
	
	void LogAdapters();								// �������������
	void LogAdapterOutputs(IDXGIAdapter* adapter);	// ���ָ����������Ϣ
	void LogOutputDisplayMode(IDXGIOutput* output, DXGI_FORMAT format);	// ����ָ����ɫ��ʽ����Կ���Ϣ

protected:
	static D3DApp* mApp;

	HINSTANCE	mhAppInst = nullptr;
	HWND		mhMainWnd = nullptr;
	bool		mAppPaused = false;		// ��ǰ�����Ƿ���ͣ
	bool		mMinimized = false;		// ��ǰ�����Ƿ���С��
	bool		mMaximized = false;		// ��ǰ�����Ƿ����
	bool		mResizing = false;		// ��ǰ���������������ô�С
	bool		mFullscreenState = false;	// ��ǰ�����Ƿ�ȫ��״̬

	bool		m4xMsaaState = false;	// �Ƿ���4x���ز���
	UINT		m4xMsaaQuality = 0;		// ���ز�������

	//GameTimer	mTimer;					// ʱ���ʱ��

	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;	// DXGI ����
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;	// ������
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;	// �豸

	Microsoft::WRL::ComPtr<ID3D12Fence>	mFence;			// Χ��
	UINT64 mCurrentFence = 0;							// Χ�����


	Microsoft::WRL::ComPtr<ID3D12CommandQueue>	mCommandQueue;	// �������
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator;	// �����ڴ������
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>	mCommandList;	// �����б�

	static const int SwapChainBufferCount = 2;			// ��Ⱦ������� �����ʾʹ��˫����
	int mCurrentBackBuffer = 0;							// ��ǰ��̨��������
	
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];	// ˫����
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;		// ��Ȼ���

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;	// Render Target View ��������
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;	// Depth Stencil View ��������

	D3D12_VIEWPORT mScreenViewport;							// �ӿڣ���������Ⱦ����Ļ������
	D3D12_RECT mScissorRect;								// ���þ��Σ����������ӿ��ڿɼ��ľ�������

	UINT mRtvDescriptorSize = 0;							// Rtv ��������С ��ͬƽ̨������ͬ
	UINT mDsvDescriptorSize = 0;							// Dsv ��������С
	UINT mCbvSrvUavDescriptorSize = 0;						// Cbv Srv Uav ��������С

	std::wstring mMainWndCaption = L"d3d App";							// �����ڱ���
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;			// Direct3D �������ͣ�ͨ����Ӳ������
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;			// ��̨�����������ظ�ʽ
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;	// ���ģ�建�����ĸ�ʽ
	
	int mClientWidth = 800;
	int mClientHeight = 600;
};

