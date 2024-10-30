#include "Renderer.h"

#include <DirectXMath.h>
#include <DirectXColors.h>

#include "../ReadData/ReadData.h"
#include "../Text2D/text2D.h"

#include "../Constant Buffers/CBufferWVP.h"
#include "../Vertex/Vertex.h"

#include <WICTextureLoader.h>

#include <DDSTextureLoader.h>

using namespace DirectX;

struct CBufferSkybox
{
	XMMATRIX WVP;
};

Renderer::Renderer() : pDevice(nullptr), pDeviceContext(nullptr), pSwapChain(nullptr), pRenderTargetView(nullptr), pZBuffer(nullptr),
					   pVertexBuffer(nullptr), pIndexBuffer(nullptr), pConstantBuffer0(nullptr), pInputLayout(nullptr), pVertexShader(nullptr), 
					   pPixelShader(nullptr), pTexture(nullptr), pSamplerState(nullptr), width(0), height(0), camera(), cube1(), cube2(), 
					   viewport(), spriteFont(nullptr), spriteBatch(nullptr), hWndCache(nullptr), rc(), pDepthStencilState(nullptr) 

{
	ambientLightColour = { 0.1f, 0.1f, 0.1f, 1.0f };
	directionalLightColour = { 0.2788f, 0.7063f, 0.6506f, 1.0f };
	directionalLightShinesFrom = { 0.96f, 0.8f, 0.75f, 0.0f };

}

HRESULT Renderer::Init(HWND hWnd)
{
	HRESULT hr = {};

	hWndCache = hWnd;

	// INFO: Get window size
	rc = {};
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
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (FLOAT)width;
	viewport.Height = (FLOAT)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	pDeviceContext->RSSetViewports(1, &viewport);

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

	// INFO: Text Rendering (Different Different Way)
	spriteBatch = std::make_unique<SpriteBatch>(pDeviceContext);
	spriteFont = std::make_unique<SpriteFont>(pDevice, L"Assets/Fonts/myfile.spritefont");

	// INFO: Initialize Text2D (Bad Way)
	//pText = new Text2D("Assets/Fonts/font1.png", pDevice, pDeviceContext);

	// INFO: Text Rendering (Good Way) Independent Resources
	//hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pD2DFactory);
	//if (FAILED(hr))
	//{
	//	return hr;
	//}
	//
	//hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&pDWriteFactory));
	//if (FAILED(hr))
	//{
	//	return hr;
	//}
	//
	//renderText = L"Health Amount: 50";
	//renderTextLength = (UINT32)wcslen(renderText);
	//
	//hr = pDWriteFactory->CreateTextFormat(L"Gabriola", NULL, DWRITE_FONT_WEIGHT_REGULAR, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 72.0f, L"en-us", &pTextFormat);
	//if (FAILED(hr))
	//{
	//	return hr;
	//}
	//
	//pTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	//pTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);

	// INFO: Text Rendering (Good Way) Dependent Resources
	//D2D1_SIZE_U size = D2D1::SizeU(rc.right - rc.left, rc.bottom - rc.top);
	//if (!pHwndRenderTarget)
	//{
	//	hr = pD2DFactory->CreateHwndRenderTarget(D2D1::RenderTargetProperties(), D2D1::HwndRenderTargetProperties(hWndCache, size), &pHwndRenderTarget);
	//	if (FAILED(hr))
	//	{
	//		return hr;
	//	}
	//}
	//
	//hr = pHwndRenderTarget->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::Black), &pBlackBrush);
	
	/*
	// INFO: Load the compiled vertex shader
	auto vertexShaderBytecode = DX::ReadData(L"Compiled Shaders/VertexShader.cso");

	// INFO: Create vertex shader object
	hr = pDevice->CreateVertexShader(vertexShaderBytecode.data(), vertexShaderBytecode.size(), NULL, &pVertexShader);
	if (FAILED(hr))
	{
		return hr;
	}

	// INFO: Create a vertex shader reflection object
	ID3D11ShaderReflection* pVertexShaderReflection = nullptr;
	hr = D3DReflect(vertexShaderBytecode.data(), vertexShaderBytecode.size(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create vertex shader reflection object\n");
		return hr;
	}

	// INFO: Get vertex shader description
	D3D11_SHADER_DESC vertexShaderDescription = {};
	pVertexShaderReflection->GetDesc(&vertexShaderDescription);

	// INFO: Get input parameter descriptions
	D3D11_SIGNATURE_PARAMETER_DESC* pSignatureParameterDescriptions = new D3D11_SIGNATURE_PARAMETER_DESC[vertexShaderDescription.InputParameters]{ 0 };
	for (UINT i = 0; i < vertexShaderDescription.InputParameters; i++)
	{
		pVertexShaderReflection->GetInputParameterDesc(i, &pSignatureParameterDescriptions[i]);
	}

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

	// INFO: Release temporay heap memory
	delete[] pSignatureParameterDescriptions;
	delete[] ied;
	
	// INFO: Load the compiled pixel shader
	auto pixelShaderBytecode = DX::ReadData(L"Compiled Shaders/PixelShader.cso");

	// INFO: Create pixel shader object
	hr = pDevice->CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), NULL, &pPixelShader);
	if (FAILED(hr))
	{
		return hr;
	}*/
	
	// INFO: VS and PS for cubes
	LoadVertexShader(L"Compiled Shaders/VertexShader.cso", &pVertexShader, &pInputLayout);
	LoadPixelShader(L"Compiled Shaders/PixelShader.cso", &pPixelShader);

	// INFO: VS and PS for Skybox
	LoadVertexShader(L"Compiled Shaders/SkyboxVShader.cso", &pVSSkybox, &pInputLayoutSkybox);
	LoadPixelShader(L"Compiled Shaders/SkyboxPShader.cso", &pPSSkybox);

	// INFO: Set the shaders (Usually done per draw call)
	//pDeviceContext->VSSetShader(pVertexShader, NULL, 0);
	//pDeviceContext->PSSetShader(pPixelShader, NULL, 0);

	// INFO: Create input layout description (Manually)
	//D3D11_INPUT_ELEMENT_DESC ied[] = 
	//{
	//	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	//	{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	//};

	// INFO: Set the input layout (Usually done per draw call)
	//pDeviceContext->IASetInputLayout(pInputLayout);

	// INFO: Create blend state descriptions
	//D3D11_BLEND_DESC blendDescEnable = {};
	//blendDescEnable.RenderTarget[0].BlendEnable = TRUE;
	//blendDescEnable.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendDescEnable.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//blendDescEnable.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendDescEnable.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendDescEnable.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//blendDescEnable.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//blendDescEnable.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//blendDescEnable.IndependentBlendEnable = FALSE;
	//blendDescEnable.AlphaToCoverageEnable = FALSE;
	//pDevice->CreateBlendState(&blendDescEnable, &pAlphaBlendStateEnable);
	//
	//D3D11_BLEND_DESC blendDescDisable = {};
	//blendDescDisable.RenderTarget[0].BlendEnable = FALSE;
	//blendDescDisable.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	//blendDescDisable.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	//blendDescDisable.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	//blendDescDisable.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	//blendDescDisable.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	//blendDescDisable.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	//blendDescDisable.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	//blendDescDisable.IndependentBlendEnable = FALSE;
	//blendDescDisable.AlphaToCoverageEnable = FALSE;
	//pDevice->CreateBlendState(&blendDescDisable, &pAlphaBlendStateDisable);

	// INFO: Back-face culling
	D3D11_RASTERIZER_DESC rasterDesc = {};
	ZeroMemory(&rasterDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	pDevice->CreateRasterizerState(&rasterDesc, &pRasterSolid); // Check with if (FAILED(hr))

	// INFO: Front-face culling
	rasterDesc.CullMode = D3D11_CULL_FRONT;
	pDevice->CreateRasterizerState(&rasterDesc, &pRasterSkyBox); // Check with if (FAILED(hr))

	// INFO: Depth Writing Enabled
	D3D11_DEPTH_STENCIL_DESC depthDesc = { 0 };
	depthDesc.DepthEnable = true;
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthDesc.DepthFunc = D3D11_COMPARISON_LESS;
	pDevice->CreateDepthStencilState(&depthDesc, &pDepthWriteSolid); // Check with if (FAILED(hr))

	// INFP: Depth writing disabled
	depthDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	pDevice->CreateDepthStencilState(&depthDesc, &pDepthWriteSkyBox); // Check with if (FAILED(hr))

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
		{ XMFLOAT3{-0.5f, -0.5f, -0.5f},	XMFLOAT4{Colors::White},	XMFLOAT2{0.0f, 1.0f},	XMFLOAT3{-0.5773f, -0.5773f, -0.5773f}},// FRONT BL
		{ XMFLOAT3{-0.5f, 0.5f, -0.5f},		XMFLOAT4{Colors::White},	XMFLOAT2{0.0f, 0.0f},	XMFLOAT3{-0.5773f, 0.5773f, -0.5773f}}, // FRONT TL
		{ XMFLOAT3{0.5f, 0.5f, -0.5f},		XMFLOAT4{Colors::White},	XMFLOAT2{1.0f, 0.0f},	XMFLOAT3{0.5773f, 0.5773f, -0.5773f}}, // FRONT TR
		{ XMFLOAT3{0.5f, -0.5f, -0.5f},		XMFLOAT4{Colors::White},	XMFLOAT2{1.0f, 1.0f},	XMFLOAT3{0.5773f, -0.5773f, -0.5773f}}, // FRONT BR
																							
		{ XMFLOAT3{-0.5f, -0.5f, 0.5f},		XMFLOAT4{Colors::White},	XMFLOAT2{0.0f, 1.0f},	XMFLOAT3{-0.5773f, -0.5773f, 0.5773f}}, // BACK BL
		{ XMFLOAT3{-0.5f, 0.5f, 0.5f},		XMFLOAT4{Colors::White},	XMFLOAT2{0.0f, 0.0f},	XMFLOAT3{-0.5773f, 0.5773f, 0.5773f}}, // BACK TL
		{ XMFLOAT3{0.5f, 0.5f, 0.5f},		XMFLOAT4{Colors::White},	XMFLOAT2{1.0f, 0.0f},	XMFLOAT3{0.5773f, 0.5773f, 0.5773f}}, // BACK TR
		{ XMFLOAT3{0.5f, -0.5f, 0.5f},		XMFLOAT4{Colors::White},	XMFLOAT2{1.0f, 1.0f},	XMFLOAT3{0.5773f, -0.5773f, 0.5773f}} // BACK BR
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

	// INFO: Create Constant Buffer for skybox
	cbd.ByteWidth = sizeof(CBufferSkybox);
	if (FAILED(pDevice->CreateBuffer(&cbd, NULL, &pSkyboxCBuffer)))
	{
		OutputDebugString(L"Failed to create skybox constant buffer\n");
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

	// INFO: Load textures
	hr = CreateWICTextureFromFile(pDevice, pDeviceContext, L"Assets/Textures/Box.bmp", nullptr, &pTexture);
	CreateWICTextureFromFile(pDevice, pDeviceContext, L"Assets/Textures/funny.jpg", nullptr, &pTexture2);

	// INFO: Create sampler description
	D3D11_SAMPLER_DESC samplerDesc = {};
	ZeroMemory(&samplerDesc, sizeof(D3D11_SAMPLER_DESC));
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	pDevice->CreateSamplerState(&samplerDesc, &pSamplerState);

	// INFO: Load Model
	model = new ObjFileModel{ (char*)"Assets/Models/Sphere.obj", pDevice, pDeviceContext };

	// INFO: Load skybox cube map
	hr = CreateDDSTextureFromFile(pDevice, pDeviceContext, L"Assets/skybox01.dds", NULL, &pSkyboxTexture);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create DDS Texture\n");
		return;
	}
}

void Renderer::InitScene()
{
	cube2.SetPosition({ 2.0f, 0.0f, 3.0f });
	cube1.SetPosition({ 0.0f, 0.0f, 1.5f });
	cube2.SetRotation({ 0, XMConvertToRadians(-45), 0 });

	cube3.SetPosition({ -2.0f, 0.0f, 3.0f });
	cube3.SetRotation({ 0, XMConvertToRadians(-45), XMConvertToRadians(180) });

	// INFO: Set up point lights
	pointLights[0] = { XMVECTOR{1.5f, 0.0f, -1.0f}, {0.9f, 0.0f, 0.85f, 1.0f}, 10, true };
	pointLights[1] = { XMVECTOR{-1.5f, 0.0f, -1.0f}, {0.0f, 0.9f, 0.85f, 1.0f}, 20, true };
}

HRESULT Renderer::LoadVertexShader(LPCWSTR filename, ID3D11VertexShader** vs, ID3D11InputLayout** il)
{
	HRESULT hr;

	// INFO: Load the compiled vertex shader
	auto vertexShaderBytecode = DX::ReadData(filename);

	// INFO: Create vertex shader object
	hr = pDevice->CreateVertexShader(vertexShaderBytecode.data(), vertexShaderBytecode.size(), NULL, vs);
	if (FAILED(hr))
	{
		return hr;
	}

	// INFO: Create a vertex shader reflection object
	ID3D11ShaderReflection* pVertexShaderReflection = nullptr;
	hr = D3DReflect(vertexShaderBytecode.data(), vertexShaderBytecode.size(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection);
	if (FAILED(hr))
	{
		OutputDebugString(L"Failed to create vertex shader reflection object\n");
		return hr;
	}

	// INFO: Get vertex shader description
	D3D11_SHADER_DESC vertexShaderDescription = {};
	pVertexShaderReflection->GetDesc(&vertexShaderDescription);

	// INFO: Get input parameter descriptions
	D3D11_SIGNATURE_PARAMETER_DESC* pSignatureParameterDescriptions = new D3D11_SIGNATURE_PARAMETER_DESC[vertexShaderDescription.InputParameters]{ 0 };
	for (UINT i = 0; i < vertexShaderDescription.InputParameters; i++)
	{
		pVertexShaderReflection->GetInputParameterDesc(i, &pSignatureParameterDescriptions[i]);
	}

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
	hr = pDevice->CreateInputLayout(ied, vertexShaderDescription.InputParameters, vertexShaderBytecode.data(), vertexShaderBytecode.size(), il);
	if (FAILED(hr))
	{
		OutputDebugString(L"Fialed to create input layout\n");
		return hr;
	}

	// INFO: Release temporay heap memory
	delete[] pSignatureParameterDescriptions;
	delete[] ied;

	return S_OK;
}

HRESULT Renderer::LoadPixelShader(LPCWSTR filename, ID3D11PixelShader** ps)
{
	HRESULT hr;

	// INFO: Load the compiled pixel shader
	auto pixelShaderBytecode = DX::ReadData(filename);

	// INFO: Create pixel shader object
	hr = pDevice->CreatePixelShader(pixelShaderBytecode.data(), pixelShaderBytecode.size(), NULL, ps);
	if (FAILED(hr))
	{
		return hr;
	}

	return S_OK;
}

void Renderer::DrawSkybox()
{
	// INFO: Front-face culling and disable depth write
	pDeviceContext->OMSetDepthStencilState(pDepthWriteSkyBox, 1);
	pDeviceContext->RSSetState(pRasterSkyBox);

	// INFO: Set skybox shaders
	pDeviceContext->VSSetShader(pVSSkybox, 0, 0);
	pDeviceContext->PSSetShader(pPSSkybox, 0, 0);
	pDeviceContext->IASetInputLayout(pInputLayoutSkybox);

	// INFO: Constant buffer data
	CBufferSkybox cbuf;
	XMMATRIX translation, projection, view;
	translation = XMMatrixTranslation(camera.GetPosition().x, camera.GetPosition().y, camera.GetPosition().z);
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), width / static_cast<float>(height), 0.1f, 100);
	view = camera.GetViewMatrix();
	cbuf.WVP = translation * view * projection;
	pDeviceContext->UpdateSubresource(pSkyboxCBuffer, 0, 0, &cbuf, 0, 0);

	// INFO: Set shader resources
	pDeviceContext->VSSetConstantBuffers(0, 1, &pSkyboxCBuffer);
	pDeviceContext->PSSetSamplers(0, 1, &pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &pSkyboxTexture);

	model->Draw();

	// INFO: Back-face culling and enable depth write
	pDeviceContext->OMSetDepthStencilState(pDepthWriteSolid, 1);
	pDeviceContext->RSSetState(pRasterSolid);

	// INFO: Set standard shaders back
	pDeviceContext->VSSetShader(pVertexShader, 0, 0);
	pDeviceContext->PSSetShader(pPixelShader, 0, 0);
	pDeviceContext->IASetInputLayout(pInputLayout);
}

void Renderer::Clean()
{
	if (pRasterSolid) pRasterSolid->Release();
	if (pRasterSkyBox) pRasterSkyBox->Release();
	if (pDepthWriteSolid) pDepthWriteSolid->Release();
	if (pDepthWriteSkyBox) pDepthWriteSkyBox->Release();
	if (pSkyboxCBuffer) pSkyboxCBuffer->Release();
	if (pSkyboxTexture) pSkyboxTexture->Release();
	if (pVSSkybox) pVSSkybox->Release();
	if (pPSSkybox) pPSSkybox->Release();

	delete model;
	spriteFont.reset();
	//if (pTextFormat) pTextFormat->Release();
	//if (pDWriteFactory) pDWriteFactory->Release();
	//if (pBlackBrush) pBlackBrush->Release();
	//if (pHwndRenderTarget) pHwndRenderTarget->Release();
	//if (pD2DFactory) pD2DFactory->Release();
	//if (pAlphaBlendStateDisable) pAlphaBlendStateDisable->Release();
	//if (pAlphaBlendStateEnable) pAlphaBlendStateEnable->Release();
	//if (pText) delete pText; pText = nullptr;
	if (pTexture) pTexture->Release();
	if (pTexture2) pTexture2->Release();
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
	// INFO: Move cubes to show lighting better
	static float t = 0;
	t += 0.0005f;

	cube1.SetPosition({ sin(t), cube1.GetPosition().y, cube1.GetPosition().z });
	cube1.SetRotation({ sin(t) * 0.75f, cos(t) * 0.75f, cube1.GetRotation().z });

	// INFO: Clear the back buffer to a solid color and clear the depth buffer
	pDeviceContext->ClearRenderTargetView(pRenderTargetView, Colors::DarkSeaGreen);
	pDeviceContext->ClearDepthStencilView(pZBuffer, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	DrawSkybox();

	// INFO: Reset to use depth buffer otherwise the text method fucks it
	pDeviceContext->OMSetDepthStencilState(pDepthStencilState, 1);
	pDeviceContext->OMSetRenderTargets(1, &pRenderTargetView, pZBuffer);
	pDeviceContext->RSSetViewports(1, &viewport);

	// INFO: Set the shaders (Usually done per draw call) so that spritebatch doesn't override it forever
	pDeviceContext->VSSetShader(pVertexShader, NULL, 0);
	pDeviceContext->PSSetShader(pPixelShader, NULL, 0);

	// INFO: Select which vertex, index buffer and primitive topology to use (PER MESH)
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
	pDeviceContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// INFO: Set the input layout (Usually done per draw call)
	pDeviceContext->IASetInputLayout(pInputLayout);

	// INFO: Set primitive topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	XMMATRIX world, view, projection;

	// INFO: Camera Projection
	projection = XMMatrixPerspectiveFovLH(XMConvertToRadians(60), width / (float)height, 0.01f, 100.0f);

	// INFO: Camera Viewing
	view = camera.GetViewMatrix();

	// INFO: Set Active Texture and Sampler
	pDeviceContext->PSSetSamplers(0, 1, &pSamplerState);
	pDeviceContext->PSSetShaderResources(0, 1, &pTexture);
	
	// INFO: Constant Buffer and World Matrix (PER OBJECT)
	CBUFFER0 cBuffer0 = {};
	world = cube1.GetWorldMatrix();
	cBuffer0.WVP = world * view * projection;

	// INFO: Lighting

	// INFO: Ambient Light
	cBuffer0.ambientLightCol = ambientLightColour;

	// INFO: Directional Light
	cBuffer0.directionalLightCol = directionalLightColour;

	// INFO: Point Light
	for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		if (!pointLights[i].enabled) continue;

		XMMATRIX inverse = XMMatrixInverse(nullptr, world);

		cBuffer0.pointLights[i].position = XMVector3Transform(pointLights[i].position, inverse);
		cBuffer0.pointLights[i].colour = pointLights[i].colour;
		cBuffer0.pointLights[i].strength = pointLights[i].strength;
		cBuffer0.pointLights[i].enabled = pointLights[i].enabled;
	}

	// INFO: Transpose rotations
	XMMATRIX transpose = XMMatrixTranspose(world);
	cBuffer0.directionalLightDir = XMVector3Transform(directionalLightShinesFrom, transpose);

	// INFO: Update Constant Buffer
	pDeviceContext->UpdateSubresource(pConstantBuffer0, 0, 0, &cBuffer0, 0, 0);
	
	// INFO: Set Constant Buffer
	pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer0);

	// INFO: Draw the index buffer to the back buffer
	pDeviceContext->DrawIndexed(36, 0, 0);


	// Exercise 1: Cycle through colors
	//FLOAT customColor[4] = { (FLOAT)(rand() / (FLOAT)RAND_MAX), (FLOAT)(rand() / (FLOAT)RAND_MAX), (FLOAT)(rand() / (FLOAT)RAND_MAX), 1.0f};
	//pDeviceContext->ClearRenderTargetView(pRenderTargetView, customColor);
	//Sleep(100); // INFO: Artificial delay to see the color change

	// Exercise 2: Change color based on key press
	//pDeviceContext->ClearRenderTargetView(pRenderTargetView, color);

	// INFO: Do 3D rendering on the back buffer here

	// INFO: Draw the vertex buffer to the back buffer
	//pDeviceContext->Draw(3, 0);


	// INFO: Duplicate for cube 2
	world = cube2.GetWorldMatrix();
	cBuffer0.WVP = world * view * projection;

	// INFO: Lighting

	// INFO: Ambient Light
	cBuffer0.ambientLightCol = ambientLightColour;

	// INFO: Directional Light
	cBuffer0.directionalLightCol = directionalLightColour;

	// INFO: Point Light
	for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		if (!pointLights[i].enabled) continue;

		XMMATRIX inverse = XMMatrixInverse(nullptr, world);

		cBuffer0.pointLights[i].position = XMVector3Transform(pointLights[i].position, inverse);
		cBuffer0.pointLights[i].colour = pointLights[i].colour;
		cBuffer0.pointLights[i].strength = pointLights[i].strength;
		cBuffer0.pointLights[i].enabled = pointLights[i].enabled;
	}

	// INFO: Transpose rotations
	transpose = XMMatrixTranspose(world);
	cBuffer0.directionalLightDir = XMVector3Transform(directionalLightShinesFrom, transpose);

	pDeviceContext->UpdateSubresource(pConstantBuffer0, 0, 0, &cBuffer0, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer0);
	pDeviceContext->DrawIndexed(36, 0, 0);

	// INFO: Duplicate for cube 3
	pDeviceContext->PSSetShaderResources(0, 1, &pTexture2);

	world = cube3.GetWorldMatrix();
	cBuffer0.WVP = world * view * projection;

	// INFO: Lighting

	// INFO: Ambient Light
	cBuffer0.ambientLightCol = ambientLightColour;

	// INFO: Directional Light
	cBuffer0.directionalLightCol = directionalLightColour;

	// INFO: Point Light
	for (size_t i = 0; i < MAX_POINT_LIGHTS; i++)
	{
		if (!pointLights[i].enabled) continue;

		XMMATRIX inverse = XMMatrixInverse(nullptr, world);

		cBuffer0.pointLights[i].position = XMVector3Transform(pointLights[i].position, inverse);
		cBuffer0.pointLights[i].colour = pointLights[i].colour;
		cBuffer0.pointLights[i].strength = pointLights[i].strength;
		cBuffer0.pointLights[i].enabled = pointLights[i].enabled;
	}

	// INFO: Transpose rotations
	transpose = XMMatrixTranspose(world);
	cBuffer0.directionalLightDir = XMVector3Transform(directionalLightShinesFrom, transpose);

	pDeviceContext->UpdateSubresource(pConstantBuffer0, 0, 0, &cBuffer0, 0, 0);
	pDeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer0);

	model->Draw();

	// INFO: Add Text (Bad Way)
	//pText->AddText("Hello World", -1.0f, 1.0f, 0.075f);

	// INFO: Render Text with Transparency (Alpha Blending)
	//pDeviceContext->OMSetBlendState(pAlphaBlendStateEnable, NULL, 0xFFFFFFFF);
	//pText->RenderText();
	//pDeviceContext->OMSetBlendState(pAlphaBlendStateDisable, NULL, 0xFFFFFFFF);

	// INFO: Draw Text (Good Way)
	//float dpi = GetDpiForWindow(hWndCache);
	//D2D1_RECT_F layoutRect = D2D1::RectF(rc.left / dpi, rc.top / dpi, (rc.right - rc.left) / dpi, (rc.bottom - rc.top) / dpi);

	//pHwndRenderTarget->DrawTextW(renderText, renderTextLength, pTextFormat, layoutRect, pBlackBrush);

	// INFO: Draw Text(Different Different Way)
	spriteBatch->Begin();
	spriteFont->DrawString(spriteBatch.get(), L"Hello world!", XMFLOAT2(0, 0), DirectX::Colors::White, 0.0f, XMFLOAT2(0.0f, 0.0f), XMFLOAT2(1.0f, 1.0f));
	spriteBatch->End();

	// INFO: Present the back buffer to the screen
	pSwapChain->Present(0, 0);
}
