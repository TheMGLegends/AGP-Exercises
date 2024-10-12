#include "Renderer.h"

#include <DirectXMath.h>
#include <DirectXColors.h>

#include "../ReadData/ReadData.h"
#include "../WIC/WICTextureLoader.h"

#include "../Constant Buffers/CBufferWVP.h"
#include "../Vertex/Vertex.h"

using namespace DirectX;

Renderer::Renderer() : pDevice(nullptr), pDeviceContext(nullptr), pSwapChain(nullptr), pRenderTargetView(nullptr), pZBuffer(nullptr),
					   pVertexBuffer(nullptr), pIndexBuffer(nullptr), pConstantBuffer0(nullptr), pInputLayout(nullptr), pVertexShader(nullptr), 
					   pPixelShader(nullptr), pTexture(nullptr), pSamplerState(nullptr), width(0), height(0), camera(), cube1(), cube2()
{
}

HRESULT Renderer::Init(HWND hWnd)
{
	// INFO: Get window size
	RECT rc = {};
	GetClientRect(hWnd, &rc);
	width = rc.right - rc.left;
	height = rc.bottom - rc.top;

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

	// INFO: Create z buffer texture description
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = width;
	descDepth.Height = height;
	descDepth.ArraySize = 1;
	descDepth.MipLevels = 1;
	descDepth.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDepth.SampleDesc.Count = scd.SampleDesc.Count;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.Usage = D3D11_USAGE_DEFAULT;

	// INFO: Create the z buffer texture
	ID3D11Texture2D* pZBufferTexture = nullptr;
	hr = pDevice->CreateTexture2D(&descDepth, NULL, &pZBufferTexture);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create depth buffer texture\n");
		return E_FAIL;
	}

	// INFO: Create depth buffer view description
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV;
    ZeroMemory(&descDSV, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
	descDSV.Format = descDepth.Format;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	hr = pDevice->CreateDepthStencilView(pZBufferTexture, &descDSV, &pZBuffer);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create depth buffer view\n");
		return E_FAIL;
	}

	// INFO: Release temporary z buffer texture
	pZBufferTexture->Release();

	// INFO: Set the render target as the back buffer
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pZBuffer);

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

	// INFO: Initialize the scene
	InitScene();

	return S_OK;
}

HRESULT Renderer::InitPipeline()
{
	HRESULT hr = {};

	// INFO: Load the compiled vertex shader & pixel shader
	auto vertexShaderBytecode = DX::ReadData(L"Compiled Shaders/VertexShader.cso");
	auto pixelShaderBytecode = DX::ReadData(L"Compiled Shaders/PixelShader.cso");

	// INFO: Create vertex shader object
	hr = pDevice->CreateVertexShader(vertexShaderBytecode.data(), vertexShaderBytecode.size(), NULL, &pVertexShader);
	if (FAILED(hr))
	{
		return hr;
	}

	// INFO: Create pixel shader object
	hr = pDevice->CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), NULL, &pPixelShader);
	if (FAILED(hr))
	{
		return hr;
	}

	// INFO: Set the shaders (Usually done per draw call)
	pDeviceContext->VSSetShader(pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader, NULL, 0);

	// INFO: Create a vertex shader reflection object
	ID3D11ShaderReflection* pVertexShaderReflection = nullptr;
	hr = D3DReflect(vertexShaderBytecode.data(), vertexShaderBytecode.size(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create vertex shader reflection object\n");
		return hr;
	}

	// INFO: Get shader description
	D3D11_SHADER_DESC vertexShaderDescription = {};
	pVertexShaderReflection->GetDesc(&vertexShaderDescription);

	// INFO: Get input parameter descriptions
	D3D11_SIGNATURE_PARAMETER_DESC* pSignatureParameterDescriptions = new D3D11_SIGNATURE_PARAMETER_DESC[vertexShaderDescription.InputParameters]{ 0 };
	for (UINT i = 0; i < vertexShaderDescription.InputParameters; i++)
	{
		pVertexShaderReflection->GetInputParameterDesc(i, &pSignatureParameterDescriptions[i]);
	}

	// INFO: Create input layout description (Manually)
	//D3D11_INPUT_ELEMENT_DESC ied[] = 
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};

	// INFO: Create input layout description (Programmatically)
	D3D11_INPUT_ELEMENT_DESC* ied = new D3D11_INPUT_ELEMENT_DESC[vertexShaderDescription.InputParameters]{ 0 };
	for (size_t i = 0; i < vertexShaderDescription.InputParameters; i++)
	{
		ied[i].SemanticName = pSignatureParameterDescriptions[i].SemanticName;
		ied[i].SemanticIndex = pSignatureParameterDescriptions[i].SemanticIndex;

		if (pSignatureParameterDescriptions[i].ComponentType == D3D_REGISTER_COMPONENT_FLOAT32)
		{
			switch (pSignatureParameterDescriptions[i].Mask)
			{
			case 1: ied[i].Format = DXGI_FORMAT_R32_FLOAT; break; // float1
			case 3: ied[i].Format = DXGI_FORMAT_R32G32_FLOAT; break; // float2
			case 7: ied[i].Format = DXGI_FORMAT_R32G32B32_FLOAT; break; // float3
			case 15: ied[i].Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break; // float4
			default: break;
			}
		}

		ied[i].InputSlot = 0;
		ied[i].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
		ied[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		ied[i].InstanceDataStepRate = 0;
	}

	// INFO: Create input layout object
	hr = pDevice->CreateInputLayout(ied, vertexShaderDescription.InputParameters, vertexShaderBytecode.data(), vertexShaderBytecode.size(), &pInputLayout);
	if (FAILED(hr))
	{
		OutputDebugString(L"Fialed to create input layout\n");
		return hr;
	}

	// INFO: Set the input layout (Usually done per draw call)
	pDeviceContext->IASetInputLayout(pInputLayout);

	// INFO: Release temporay heap memory
	delete[] pSignatureParameterDescriptions;
	delete[] ied;

	return S_OK;
}

void Renderer::InitGraphics()
{
	HRESULT hr = {};

	// INFO: Create Vertices Array to represent a triangle
	//Vertex vertices[] =
	//{
	//	{ XMFLOAT3{-0.5f, -0.5f, 0.0f}, XMFLOAT4{Colors::Red} },
	//	{ XMFLOAT3{0.0f, 0.5f, 0.0f}, XMFLOAT4{Colors::Green} },
	//	{ XMFLOAT3{0.5f, -0.5f, 0.0f}, XMFLOAT4{Colors::Blue} }
	//};

	// INFO: Create Vertices Array to represent a cube
	Vertex vertices[] =
	{
		{ XMFLOAT3{-0.5f, -0.5f, -0.5f},	XMFLOAT4{Colors::Red},			XMFLOAT2{0.0f, 1.0f} },// FRONT BL
		{ XMFLOAT3{-0.5f, 0.5f, -0.5f},		XMFLOAT4{Colors::Green},		XMFLOAT2{0.0f, 0.0f} }, // FRONT TL
		{ XMFLOAT3{0.5f, 0.5f, -0.5f},		XMFLOAT4{Colors::Blue},			XMFLOAT2{1.0f, 0.0f } }, // FRONT TR
		{ XMFLOAT3{0.5f, -0.5f, -0.5f},		XMFLOAT4{Colors::Aqua},			XMFLOAT2{1.0f, 1.0f} }, // FRONT BR
		
		{ XMFLOAT3{-0.5f, -0.5f, 0.5f},		XMFLOAT4{Colors::Beige},		XMFLOAT2{0.0f, 1.0f} }, // BACK BL
		{ XMFLOAT3{-0.5f, 0.5f, 0.5f},		XMFLOAT4{Colors::BurlyWood},	XMFLOAT2{0.0f, 0.0f} }, // BACK TL
		{ XMFLOAT3{0.5f, 0.5f, 0.5f},		XMFLOAT4{Colors::Yellow},		XMFLOAT2{1.0f, 0.0f} }, // BACK TR
		{ XMFLOAT3{0.5f, -0.5f, 0.5f},		XMFLOAT4{Colors::Purple},		XMFLOAT2{1.0f, 1.0f} } // BACK BR
	};

	// INFO: Create vertex buffer description
	D3D11_BUFFER_DESC bd = { 0 };
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
	D3D11_MAPPED_SUBRESOURCE ms = { 0 };
	pDeviceContext->Map(pVertexBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); // INFO: Lock the buffer
	memcpy(ms.pData, vertices, sizeof(vertices)); // INFO: Copy the data
	pDeviceContext->Unmap(pVertexBuffer, NULL); // INFO: Unlock the buffer

	// INFO: Create constant buffer description
	D3D11_BUFFER_DESC cbd = { 0 };
	cbd.Usage = D3D11_USAGE_DEFAULT;
	cbd.ByteWidth = sizeof(CBUFFER0);
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	if (FAILED(pDevice->CreateBuffer(&cbd, NULL, &pConstantBuffer0)))
	{
		OutputDebugString(L"Failed to create constant buffer\n");
		return;
	}

	// INFO: Create Indices Array to represent a cube
	unsigned int indices[] =
	{
		0, 1, 2, 2, 3, 0, // Front Face
		7, 6, 5, 5, 4, 7, // Back Face
		4, 5, 1, 1, 0, 4, // Left Face
		3, 2, 6, 6, 7, 3, // Right Face
		1, 5, 6, 6, 2, 1, // Top Face
		4, 0, 3, 3, 7, 4 // Bottom Face
	};

	// INFO: Create index buffer description
	D3D11_BUFFER_DESC ibd = { 0 };
	ibd.Usage = D3D11_USAGE_DEFAULT;
	ibd.ByteWidth = sizeof(indices);
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;

	// INFO: Create subresource to hold the index data
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = indices;

	// INFO: Create the index buffer
	if (FAILED(pDevice->CreateBuffer(&ibd, &initData, &pIndexBuffer)))
	{
		OutputDebugString(L"Failed to create index buffer\n");
		return;
	}

	// INFO: Load texture
	hr = CreateWICTextureFromFile(pDevice, pDeviceContext, L"Assets/Textures/Box.bmp", nullptr, &pTexture);

	// INFO: Create sampler description
	D3D11_SAMPLER_DESC samplerDesc = {};
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	pDevice->CreateSamplerState(&samplerDesc, &pSamplerState);
}

void Renderer::InitScene()
{
	cube2.SetPosition({ 0.7f, 0.0f, 3.0f });
	cube2.SetRotation({ 0, XMConvertToRadians(45), 0 });
}

void Renderer::Clean()
{
	if (pTexture) pTexture->Release();
	if (pSamplerState) pSamplerState->Release();
	if (pZBuffer) pZBuffer->Release();
	if (pIndexBuffer) pIndexBuffer->Release();
	if (pConstantBuffer0) pConstantBuffer0->Release();
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
	// INFO: Clear the back buffer to a solid color and clear the depth buffer
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, Colors::DarkSeaGreen);
	pDeviceContext->ClearDepthStencilView(pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// INFO: Select which vertex, index buffer and primitive topology to use (PER MESH)
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// INFO: Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world, view, projection;

	// INFO: Camera Projection
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), width / (float)height, 0.01f, 100.0f);

	// INFO: Camera Viewing
	view = camera.GetViewMatrix();

	// INFO: Constant Buffer and World Matrix (PER OBJECT)
	world = cube1.GetWorldMatrix();
	CBUFFER0 cBuffer0 = {};
	cBuffer0.WVP = world * view * projection;
	
	// INFO: Update Constant Buffer
	pDeviceContext->UpdateSubresource(pConstantBuffer0, 0, 0, &cBuffer0, 0, 0);
	
	// INFO: Set Constant Buffer
	pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer0);

	// INFO: Set Active Texture and Sampler
	pDeviceContext->PSSetSamplers(0, 1, &pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);

	// Exercise 1: Cycle through colors
	//FLOAT customColor[4] = { (FLOAT)(rand() / (FLOAT)RAND_MAX), (FLOAT)(rand() / (FLOAT)RAND_MAX), (FLOAT)(rand() / (FLOAT)RAND_MAX), 1.0f};
	//pDeviceContext->ClearRenderTargetView(pRenderTargetView, customColor);
	//Sleep(100); // INFO: Artificial delay to see the color change

	// Exercise 2: Change color based on key press
	//pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

	// INFO: Do 3D rendering on the back buffer here

	// INFO: Draw the vertex buffer to the back buffer
	//pDeviceContext->Draw(3, 0);

	// INFO: Draw the index buffer to the back buffer
	pDeviceContext->DrawIndexed(36, 0, 0);

	// INFO: Duplicate for cube 2
	world = cube2.GetWorldMatrix();
	cBuffer0.WVP = world * view * projection;
	pDeviceContext->UpdateSubresource(pConstantBuffer0, 0, 0, &cBuffer0, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer0);
	pDeviceContext->DrawIndexed(36, 0, 0);

	// INFO: Present the back buffer to the screen
	pSwapChain->Present(0, 0);
}
