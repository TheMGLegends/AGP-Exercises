#pragma once

#include <DirectXMath.h>

#include "../Lights/PointLight.h"

struct CBUFFER0
{
	DirectX::XMMATRIX WVP; // 64 bytes World-View-Projection Matrix

	DirectX::XMVECTOR ambientLightCol; // 16 bytes Ambient Light Colour
	DirectX::XMVECTOR directionalLightDir; // 16 bytes Directional Light Direction
	DirectX::XMVECTOR directionalLightCol; // 16 bytes Directional Light Colour

	//DirectX::XMVECTOR pointLightPos; // 16 bytes Point Light Position
	//DirectX::XMVECTOR pointLightCol; // 16 bytes Point Light Colour
	//float pointLightStrength; // 4 bytes Point Light Strength
	PointLight pointLights[MAX_POINT_LIGHTS]; // 48 bytes * 8 = 384 bytes
};