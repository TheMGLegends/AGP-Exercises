#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <d2d1.h>
#include <dwrite.h>

#include "../Camera/Camera.h"
#include "../Transform/Transform.h"

class Text2D;

class Renderer
{
public:
	Renderer();

	HRESULT Init(HWND hWnd);
	HRESULT InitPipeline();
	void InitGraphics();
	void InitScene();

	void Clean();

	void RenderFrame();

	Camera& GetCamera() { return camera; }

private:
	HWND hWndCache;

	UINT width; // Width of the Window
	UINT height; // Height of the Window

	IDXGISwapChain* pSwapChain;
	ID3D11Device* pDevice;
	ID3D11DeviceContext* pDeviceContext;
	ID3D11RenderTargetView* pRenderTargetView; // Used to Access the Back Buffer

	ID3D11VertexShader* pVertexShader;
	ID3D11PixelShader* pPixelShader;
	ID3D11InputLayout* pInputLayout;

	ID3D11Buffer* pVertexBuffer;
	ID3D11Buffer* pConstantBuffer0;
	ID3D11Buffer* pIndexBuffer;

	ID3D11DepthStencilView* pZBuffer;

	ID3D11ShaderResourceView* pTexture;
	ID3D11SamplerState* pSamplerState;
	
	// INFO: Text Rendering (Bad Way)
	Text2D* pText;

	// INFO: Text Rendering (Good Way)
	IDWriteFactory* pDWriteFactory;
	IDWriteTextFormat* pTextFormat;

	ID2D1Factory* pD2DFactory;
	ID2D1HwndRenderTarget* pHwndRenderTarget;
	ID2D1SolidColorBrush* pBlackBrush;

	const wchar_t* renderText;
	UINT32 renderTextLength;

	RECT rc;


	ID3D11BlendState* pAlphaBlendStateEnable;
	ID3D11BlendState* pAlphaBlendStateDisable;

	// WVP Matrix Testing Variables
	Transform cube1;
	Transform cube2;

	Camera camera;
};

