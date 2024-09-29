#include "Renderer.h"

#include <DirectXColors.h>
#include <d3dcompiler.h>
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

	// INFO: Initialize the graphics pipeline
	InitPipeline();

	// INFO: Set up input data for rendering
	InitGraphics();

	return S_OK;
}

HRESULT Renderer::InitPipeline()
{
	HRESULT hr = {};
	ID3DBlob* pVertexShaderBlob = nullptr;
	ID3DBlob* pPixelShaderBlob = nullptr;
	ID3DBlob* pErrorBlob = nullptr;

	// INFO: Compile the vertex shader
	hr = D3DCompileFromFile(L"Shaders/VertexShader.hlsl", 0, 0, "main", "vs_4_0", 0, 0, &pVertexShaderBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
		pErrorBlob->Release();
		return hr;
	}

	// INFO: Compile the pixel shader
	hr = D3DCompileFromFile(L"Shaders/PixelShader.hlsl", 0, 0, "main", "ps_4_0", 0, 0, &pPixelShaderBlob, &pErrorBlob);
	if (FAILED(hr))
	{
		OutputDebugStringA(reinterpret_cast<const char*>(pErrorBlob->GetBufferPointer()));
		pErrorBlob->Release();
		return hr;
	}

	// INFO: Create vertex shader object
	hr = pDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), NULL, &pVertexShader);
	if (FAILED(hr))
	{
		pVertexShaderBlob->Release();
		return hr;
	}

	// INFO: Create pixel shader object
	hr = pDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), NULL, &pPixelShader);
	if (FAILED(hr))
	{
		pPixelShaderBlob->Release();
		return hr;
	}

	// INFO: Set the shaders (Usually done per draw call)
	pDeviceContext->VSSetShader(pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader, NULL, 0);

	// INFO: Create input layout description
	D3D11_INPUT_ELEMENT_DESC ied[] = 
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	// INFO: Create input layout object
	hr = pDevice->CreateInputLayout(ied, ARRAYSIZE(ied), pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &pInputLayout);
	pVertexShaderBlob->Release();
	pPixelShaderBlob->Release();
	if (FAILED(hr))
	{
		OutputDebugString(L"Fialed to create input layout\n");
		return hr;
	}

	// INFO: Set the input layout (Usually done per draw call)
	pDeviceContext->IASetInputLayout(pInputLayout);

	return S_OK;
}

void Renderer::InitGraphics()
{
	HRESULT hr = {};

	// INFO: Create Vertices Array to represent a triangle
	Vertex vertices[] =
	{
		{ XMFLOAT3{-0.5f, -0.5f, 0.0f}, XMFLOAT4{Colors::Red} },
		{ XMFLOAT3{0.0f, 0.5f, 0.0f}, XMFLOAT4{Colors::Green} },
		{ XMFLOAT3{0.5f, -0.5f, 0.0f}, XMFLOAT4{Colors::Blue} }
	};

	// INFO: Create vertex buffer description
	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(Vertex) * ARRAYSIZE(vertices);
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	hr = pDevice->CreateBuffer(&bd, NULL, &pVertexBuffer);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create vertex buffer\n");
		return;
	}

	// IFNO: Copy vertices into buffer
	D3D11_MAPPED_SUBRESOURCE ms = {};
	pDeviceContext->Map(pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // INFO: Lock the buffer
	memcpy(ms.pData, vertices, sizeof(vertices)); // INFO: Copy the data
	pDeviceContext->Unmap(pVertexBuffer, NULL); // INFO: Unlock the buffer
}

void Renderer::Clean()
{
	if (pVertexBuffer) pVertexBuffer->Release();
	if (pInputLayout) pInputLayout->Release();
	if (pPixelShader) pPixelShader->Release();
	if (pVertexShader) pVertexShader->Release();
	if (pRenderTargetView) pRenderTargetView->Release();
	if (pSwapChain) pSwapChain->Release();
	if (pDevice) pDevice->Release();
	if (pDeviceContext) pDeviceContext->Release();
}

void Renderer::RenderFrame()
{
	// INFO: Clear the back buffer to a solid color
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, Colors::Aqua);

	// Exercise 1: Cycle through colors
	//FLOAT customColor[4] = { (FLOAT)(rand() / (FLOAT)RAND_MAX), (FLOAT)(rand() / (FLOAT)RAND_MAX), (FLOAT)(rand() / (FLOAT)RAND_MAX), 1.0f};
	//pDeviceContext->ClearRenderTargetView(pRenderTargetView, customColor);
	//Sleep(100); // INFO: Artificial delay to see the color change

	// Exercise 2: Change color based on key press
	//pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

	// INFO: Do 3D rendering on the back buffer here

	// INFO: Tutorial 3B
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	// INFO: Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// INFO: Draw the vertex buffer to the back buffer
	pDeviceContext->Draw(3, 0);

	// INFO: Present the back buffer to the screen
	pSwapChain->Present(0, 0);
}

//void Renderer::SetColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a)
//{
//	color[0] = r;
//	color[1] = g;
//	color[2] = b;
//	color[3] = a;
//}