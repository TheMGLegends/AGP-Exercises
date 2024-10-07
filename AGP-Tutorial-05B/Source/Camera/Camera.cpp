#include "Camera.h"

#include <algorithm>

using namespace DirectX;

Camera::Camera() : position{ 0.0f, 0.0f, 0.0f }, minMaxPitch{ 1.0f, 179.0f }, pitch{ XM_PIDIV2 }, yaw{ 0.0f }
{
}

XMMATRIX Camera::GetViewMatrix()
{
	XMVECTOR eyePos{ position.x, position.y, position.z };
	XMVECTOR lookTo
	{ 
		static_cast<float>(sin(yaw) * sin(pitch)),
		static_cast<float>(cos(pitch)),
		static_cast<float>(cos(yaw) * sin(pitch))
	};
	XMVECTOR camUp{ 0, 1, 0 };

	XMMATRIX view = XMMatrixLookToLH(eyePos, lookTo, camUp);

	return view;
}

void Camera::AddPitch(float p)
{
	pitch += p;

	pitch = std::clamp(pitch, XMConvertToRadians(minMaxPitch.x), XMConvertToRadians(minMaxPitch.y));
}
