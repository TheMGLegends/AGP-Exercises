#pragma once

#include <DirectXMath.h>

struct CBUFFER0
{
	DirectX::XMMATRIX WVP; // 64 bytes World-View-Projection Matrix
	DirectX::XMVECTOR ambientLightCol; // 16 bytes Ambient Light Colour
	DirectX::XMVECTOR directionalLightDir; // 16 bytes Directional Light Direction
	DirectX::XMVECTOR directionalLightCol; // 16 bytes Directional Light Colour

};