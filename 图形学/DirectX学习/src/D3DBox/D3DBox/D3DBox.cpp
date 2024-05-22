// D3DBox.cpp : 定义应用程序的入口点。
//

#include "D3DBox.h"
#include "D3dApp.h"


D3DBox::D3DBox(HINSTANCE hInstance) : D3DApp(hInstance)
{
}

D3DBox::~D3DBox()
{
}

bool D3DBox::Initialize()
{
	if (!D3DApp::Initialize())
		return false;

	return true;
}

void D3DBox::OnResize()
{
	D3DApp::OnResize();
}

void D3DBox::Update(const GameTimer& gt)
{	
	
}

void D3DBox::Draw(const GameTimer& gt)
{

}


int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	D3DBox app(hInstance);
	if (!app.Initialize()) {
		return 0;
	}

	return app.Run();
}