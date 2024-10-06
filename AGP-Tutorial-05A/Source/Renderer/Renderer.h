#pragma once

#include <d3d11.h>
#include <DirectXMath.h>


// INFO: Define the camera structure
struct Camera
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
	float pitch = DirectX::XM_PIDIV2, yaw = 0.0f;
};

class Renderer
{
public:
	Renderer();
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

	Camera camera;

	bool moveForward = false;
	bool moveBackward = false;
	bool moveLeft = false;
	bool moveRight = false;

	bool pitchUp = false;
	bool pitchDown = false;
	bool yawLeft = false;
	bool yawRight = false;

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
	ID3D11Buffer* pIndexBuffer;

	// Testing Movement
	float xPosition;
	float yPosition;

	// WVP Matrix Testing Variables
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	UINT width; // Width of the Window
	UINT height; // Height of the Window


	// Exercise 2 Variables
	//FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

