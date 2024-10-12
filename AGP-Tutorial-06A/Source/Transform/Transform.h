#pragma once

#include <DirectXMath.h>

class Transform
{
public:
	Transform();

	DirectX::XMMATRIX GetWorldMatrix() const;

	inline DirectX::XMFLOAT3 GetPosition() const { return position; }
	inline void SetPosition(DirectX::XMFLOAT3 pos) { position = pos; }
	inline void SetPosition(float x, float y, float z) { position = { x, y, z }; }

	inline DirectX::XMFLOAT3 GetRotation() const { return rotation; }
	inline void SetRotation(DirectX::XMFLOAT3 rot) { rotation = rot; }
	inline void SetRotation(float x, float y, float z) { rotation = { x, y, z }; }

	inline DirectX::XMFLOAT3 GetScale() const { return scale; }
	inline void SetScale(DirectX::XMFLOAT3 s) { scale = s; }
	inline void SetScale(float x, float y, float z) { scale = { x, y, z }; }

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 rotation;
	DirectX::XMFLOAT3 scale;
};

