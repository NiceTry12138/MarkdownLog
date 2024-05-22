#pragma once
#include "D3dApp.h"

class D3DBox : public D3DApp
{
public:
	D3DBox(HINSTANCE hInstance);
	virtual ~D3DBox();

	virtual bool Initialize() override;
protected:

private:
	virtual void OnResize() override;
	virtual void Update(const GameTimer& gt) override;
	virtual void Draw(const GameTimer& gt) override;
};