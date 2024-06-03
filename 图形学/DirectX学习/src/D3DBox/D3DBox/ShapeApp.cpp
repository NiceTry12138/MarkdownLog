#include "ShapeApp.h"

bool ShapeApp::Initialize()
{
	if (!D3DApp::Initialize()) {
		return false;
	}

	// ���������б�
	ThrowIfFailed(mCommandList->Reset(mCommandAllocator.Get(), nullptr));



	// ִ�г�ʼ��ʱ������
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// �ȴ���ʼ���������
	FlushCommandQueue();
	return true;
}

void ShapeApp::OnResize()
{
	D3DApp::OnResize();

	// ���ڴ�С����ʱ ������ͼ����
	XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, AspectRatio(), 1.0f, 1000.0f);
	XMStoreFloat4x4(&mProj, P);
}

void ShapeApp::BuildShaderAndInputLayout()
{

}
