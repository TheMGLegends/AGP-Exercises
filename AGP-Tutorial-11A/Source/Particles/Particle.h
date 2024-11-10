#pragma once

#include <DirectXMath.h>
#include "../Transform/Transform.h"

using namespace DirectX;

struct Particle
{
	float age;
	float gravity;
	Transform transform;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT4 colour = { 0.45f, 1.0f, 0.67f, 1.0f };

	// INFO: Used to make the particle face the camera
	XMMATRIX LookAt_XZ(float targetX, float targetZ)
	{
		//XMFLOAT3 particlePos = transform.GetPosition();
		//
		//return XMMatrixLookAtLH(XMLoadFloat3(&particlePos), XMVECTOR{ targetX, particlePos.y, targetZ }, XMVectorSet(0, 1, 0, 0));

		XMFLOAT3 particlePos = transform.GetPosition();

		// Calculate the direction vector from the particle to the target
		float deltaX = targetX - particlePos.x;
		float deltaZ = targetZ - particlePos.z;

		// Calculate the angle in radians
		float angle = -atan2f(deltaZ, deltaX);

		// Create a rotation matrix around the Y-axis
		return XMMatrixRotationY(angle - XMConvertToRadians(90.0f));
	}
};

