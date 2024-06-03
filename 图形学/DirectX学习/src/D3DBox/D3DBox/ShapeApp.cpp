#include "ShapeApp.h"

bool ShapeApp::Initialize()
{
	if (!D3DApp::Initialize()) {
		return false;
	}

	// 重置命令列表
	ThrowIfFailed(mCommandList->Reset(mCommandAllocator.Get(), nullptr));



	// 执行初始化时的命令
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// 等待初始化命令完成
	FlushCommandQueue();
	return true;
}

void ShapeApp::OnResize()
{
	D3DApp::OnResize();

	// 窗口大小更新时 更新视图矩阵
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void ShapeApp::BuildShaderAndInputLayout()
{

}
