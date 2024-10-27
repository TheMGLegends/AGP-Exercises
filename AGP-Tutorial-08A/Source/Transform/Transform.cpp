#include "Transform.h"

using namespace DirectX;

Transform::Transform() : position{ 0.0f, 0.0f, 0.0f }, rotation{ 0.0f, 0.0f, 0.0f }, scale{ 1.0f, 1.0f, 1.0f }
{
}

XMMATRIX Transform::GetWorldMatrix() const
{
	XMMATRIX translationMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);

	XMMATRIX world = scaleMatrix * rotationMatrix * translationMatrix;

	return world;
}
