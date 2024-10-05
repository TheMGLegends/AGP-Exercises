#pragma once

#include <d3d11.h>

class Renderer
{
public:
	Renderer() = default;
	HRESULT Init(HWND hWnd);
	HRESULT InitPipeline();
	void InitGraphics();
	void Clean();

	void RenderFrame();

	// Testing Movement
	inline void AddXPosition(float x) { xPosition += x; }
	inline void AddYPosition(float y) { yPosition += y; }

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

	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pConstantBuffer0;

	// Testing Movement
	float xPosition;
	float yPosition;

	// Exercise 2 Variables
	//FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

