#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera();

	DirectX::XMMATRIX GetViewMatrix();

	inline DirectX::XMFLOAT3 GetPosition() const { return position; }
	inline void AddPosition(DirectX::XMFLOAT3 pos) { position.x += pos.x; position.y += pos.y; position.z += pos.z; }
	inline void AddPosition(float x, float y, float z) { position.x += x; position.y += y; position.z += z; }

	inline float GetPitch() const { return pitch; }
	void AddPitch(float p);

	inline float GetYaw() const { return yaw; }
	inline void AddYaw(float y) { yaw += y; }


private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 minMaxPitch;

	float pitch;
	float yaw;
};

