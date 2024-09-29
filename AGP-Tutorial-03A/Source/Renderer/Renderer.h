#pragma once

#include <d3d11.h>
#include <DirectXMath.h>

struct Vertex
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT4 color;
};

class Renderer
{
public:
	Renderer() = default;
	HRESULT Init(HWND hWnd);
	HRESULT InitPipeline();
	void Clean();

	void RenderFrame();

	// Exercise 2 Functions
	//void SetColor(FLOAT r, FLOAT g, FLOAT b, FLOAT a = 1.0f);

private:
	IDXGISwapChain* pSwapChain;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	ID3D11RenderTargetView* pRenderTargetView; // INFO: Used to Access the Back Buffer

	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;
	ID3D11InputLayout* pInputLayout;

	// Exercise 2 Variables
	//FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

