#pragma once

#include <d3d11.h>

class Renderer
{
public:
	HRESULT Init(HWND hWnd);
	void Clean();

private:
	IDXGISwapChain* pSwapChain;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
};

