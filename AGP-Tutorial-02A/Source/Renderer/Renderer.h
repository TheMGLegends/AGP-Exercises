#pragma once

#include <d3d11.h>

class Renderer
{
public:
	HRESULT Init(HWND hWnd);
	void Clean();

	void RenderFrame();

	// Exercise 2 Functions
	void SetColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a = 1.0f);

private:
	IDXGISwapChain* pSwapChain;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	ID3D11RenderTargetView* pRenderTargetView; // INFO: Used to Access the Back Buffer

	// Exercise 2 Variables
	FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

