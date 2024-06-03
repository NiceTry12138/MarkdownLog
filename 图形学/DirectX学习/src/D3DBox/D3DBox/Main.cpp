#include <Windows.h>
#include "D3DBox.h"

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