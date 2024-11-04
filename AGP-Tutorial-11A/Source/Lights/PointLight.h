#pragma once

#include <DirectXMath.h>
#include <minwindef.h>

struct PointLight
{
	DirectX::XMVECTOR position = { 0.0f, 0.0f, 0.0f, 1.0f }; // 16 bytes
	DirectX::XMVECTOR colour = { 1.0f, 1.0f, 1.0f, 1.0f }; // 16 bytes

	float strength = 10.0f; // 4 bytes
	BOOL enabled = false; // 4 bytes
	float padding[2]; // 8 bytes
};