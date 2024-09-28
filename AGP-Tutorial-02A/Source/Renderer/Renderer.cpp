#include "Renderer.h"

#include <DirectXColors.h>
#include <random>

using namespace DirectX;

HRESULT Renderer::Init(HWND hWnd)
{
	// INFO: Get window size
	RECT rc = {};
	GetClientRect(hWnd, &rc);
	UINT width = rc.right - rc.left;
	UINT height = rc.bottom - rc.top;

	// INFO: Create swap chain description
	DXGI_SWAP_CHAIN_DESC scd = {};

	// INFO: Fill in the swap chain description
	scd.BufferCount = 1;
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.Windowed = TRUE;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// INFO: Create device, device context and swap chain
	HRESULT hr = {};
	hr = D3D11CreateDeviceAndSwapChain(NULL, 
		D3D_DRIVER_TYPE_HARDWARE, 
		NULL, 
		D3D11_CREATE_DEVICE_DEBUG, 
		NULL, 
		NULL, 
		D3D11_SDK_VERSION, 
		&scd, 
		&pSwapChain, 
		&pDevice, 
		NULL, 
		&pDeviceContext);

	// INFO: Get the address of the back buffer from the swap chain
	ID3D11Texture2D* pBackBufferTexture = nullptr;
	hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBufferTexture);

	// INFO: Create a render target view
	if (pBackBufferTexture != nullptr)
		hr = pDevice->CreateRenderTargetView(pBackBufferTexture, NULL, &pRenderTargetView);

	// INFO: Release temporary back buffer texture
	pBackBufferTexture->Release();

	// INFO: Set the render target as the back buffer
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

	// INFO: Set the viewport
	D3D11_VIEWPORT vp = {};
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	vp.Width = (FLOAT)width;
	vp.Height = (FLOAT)height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	pDeviceContext->RSSetViewports(1, &vp);

	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

void Renderer::Clean()
{
	pRenderTargetView->Release();
	pSwapChain->Release();
	pDevice->Release();
	pDeviceContext->Release();
}

void Renderer::RenderFrame()
{
	// INFO: Clear the back buffer to a solid color
	//pDeviceContext->ClearRenderTargetView(pRenderTargetView, Colors::AliceBlue);

	// Exercise 1: Cycle through colors
	//FLOAT customColor[4] = { (FLOAT)(rand() / (FLOAT)RAND_MAX), (FLOAT)(rand() / (FLOAT)RAND_MAX), (FLOAT)(rand() / (FLOAT)RAND_MAX), 1.0f};
	//pDeviceContext->ClearRenderTargetView(pRenderTargetView, customColor);
	//Sleep(100); // INFO: Artificial delay to see the color change

	// Exercise 2: Change color based on key press
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

	// INFO: Do 3D rendering on the back buffer here

	// INFO: Present the back buffer to the screen
	pSwapChain->Present(0, 0);
}

void Renderer::SetColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
{
	color[0] = r;
	color[1] = g;
	color[2] = b;
	color[3] = a;
}
