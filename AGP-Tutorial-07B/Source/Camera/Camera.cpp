#include "Camera.h"

#include <algorithm>

using namespace DirectX;

Camera::Camera() : position{ 0.0f, 0.0f, 0.0f }, minMaxPitch{ 1.0f, 179.0f }, pitch{ XM_PIDIV2 }, yaw{ 0.0f }, 
				   moveWS{ 0.0f }, moveAD{ 0.0f }, pitchUpDown{ 0.0f }, yawLeftRight{ 0.0f }, speed { 0.001f },
				   rotationSpeed{ 0.001f }
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

void Camera::Update()
{
	if (moveWS != 0)
	{
		AddPosition
		(
			XMFLOAT3{
				static_cast<float>(moveWS * speed * sin(yaw) * sin(pitch)),
				static_cast<float>(moveWS * speed * cos(pitch)),
				static_cast<float>(moveWS * speed * cos(yaw) * sin(pitch))
			}
		);

		moveWS = 0.0f;
	}

	if (moveAD != 0)
	{
		AddPosition
		(
			XMFLOAT3{
				static_cast<float>(moveAD * speed * cos(yaw)),
				0,
				static_cast<float>(moveAD * speed * -sin(yaw))
			}
		);

		moveAD = 0.0f;
	}

	if (pitchUpDown != 0)
	{
		AddPitch(pitchUpDown * rotationSpeed);
	}

	if (yawLeftRight != 0)
	{
		AddYaw(yawLeftRight * rotationSpeed);
	}
}

void Camera::AddPitch(float p)
{
	pitch += p;

	pitch = std::clamp(pitch, XMConvertToRadians(minMaxPitch.x), XMConvertToRadians(minMaxPitch.y));
}

void Camera::ResetCamera()
{
	position = { 0.0f, 0.0f, 0.0f };
	pitch = XM_PIDIV2;
	yaw = 0.0f;
	moveWS = 0.0f;
	moveAD = 0.0f;
	pitchUpDown = 0.0f;
	yawLeftRight = 0.0f;
	speed = 0.001f;
	rotationSpeed = 0.0005f;
}
