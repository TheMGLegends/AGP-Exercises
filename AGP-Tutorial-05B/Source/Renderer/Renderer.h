#pragma once

#include <d3d11.h>
#include <DirectXMath.h>
#include <DirectXColors.h>
#include <d3dcompiler.h>
#include <d3d11shader.h>

using namespace DirectX;

// INFO: Define the camera structure
struct Camera
{
	float x = 0.0f, y = 0.0f, z = 0.0f;
	float pitch = DirectX::XM_PIDIV2, yaw = 0.0f;
	
	XMMATRIX GetViewMatrix()
	{
		XMVECTOR eyePos{ x, y, z };
		XMVECTOR lookTo{ sin(yaw) * sin(pitch),
				 cos(pitch),
				 cos(yaw) * sin(pitch) };
		XMVECTOR camUp{ 0, 1, 0 };

		XMMATRIX view = XMMatrixLookToLH(eyePos, lookTo, camUp);

		return view;
	}
};

struct Transform
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;

	XMMATRIX GetWorldMatrix()
	{
		XMMATRIX translation = XMMatrixTranslation(position.x, position.y, position.z);
		XMMATRIX rot = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
		XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

		XMMATRIX world = scaleMatrix * rot * translation;

		return world;
	}
};

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

	ID3D11DepthStencilView* pZBuffer;

	// Testing Movement
	float xPosition;
	float yPosition;

	// WVP Matrix Testing Variables
	Transform cube1;
	Transform cube2;

	UINT width; // Width of the Window
	UINT height; // Height of the Window


	// Exercise 2 Variables
	//FLOAT color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
};

