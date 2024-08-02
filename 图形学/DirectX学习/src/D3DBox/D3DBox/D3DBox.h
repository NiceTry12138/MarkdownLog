#pragma once
#include "D3dApp.h"
#include "MathHelper.h"
#include "UploadBuffer.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

struct BoxVertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
};

struct BoxObjectConstants
{
	XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};

class D3DBox : public D3DApp
{
public:
	D3DBox(HINSTANCE hInstance);
	virtual ~D3DBox();

	D3DBox& operator =(const D3DBox& rhs) = delete;
	D3DBox(const D3DBox& rhs) = delete;

	virtual bool Initialize() override;
protected:

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y);
	virtual void OnMouseUp(WPARAM btnState, int x, int y);
	virtual void OnMouseMove(WPARAM btnStae, int x, int y);

	void BuildDescriptorHeaps();	// 创建 CSV SRV UAV 堆描述符
	void BuildConstantBuffers();	// 创建 CBV 常量缓冲区视图
	void BuildRootSignature();		// 创建 根标签 用于绑定常量缓冲区
	void BuildShaderAndInputLayout();	// 创建 Shader 和输入布局
	void BuildPSO();				// 创建 Pipeline State Object 流水线状态对象，绑定流水线资源
	void BuildBoxGeometry();		// 创建盒子的顶点缓冲和索引缓冲

private:
	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	std::unique_ptr<UploadBuffer<BoxObjectConstants>> mObjectCB = nullptr;
	
	std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;	// Box 的几何坐标

	ComPtr<ID3DBlob> mVSByteCode = nullptr;				// 顶点着色器编译后的字节码
	ComPtr<ID3DBlob> mPSByteCode = nullptr;				// 像素着色器编译后的字节码

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;	// 输入布局

	ComPtr<ID3D12PipelineState> mPSO = nullptr;			// 流水线状态对象

	XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
	XMFLOAT4X4 mView = MathHelper::Identity4x4();
	XMFLOAT4X4 mProj = MathHelper::Identity4x4();

	float mTheta = 1.5f * XM_PI;
	float mPhi = XM_PIDIV4;
	float mRadius = 5.0f;

	POINT mLastMousePos;
};