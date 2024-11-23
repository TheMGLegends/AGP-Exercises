#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera();

	const DirectX::XMMATRIX GetViewMatrix();

	void Update();

	inline DirectX::XMFLOAT3 GetPosition() const { return position; }
	inline void AddPosition(DirectX::XMFLOAT3 pos) { position.x += pos.x; position.y += pos.y; position.z += pos.z; }
	inline void AddPosition(float x, float y, float z) { position.x += x; position.y += y; position.z += z; }

	inline float GetPitch() const { return pitch; }
	void AddPitch(float p);

	inline float GetYaw() const { return yaw; }
	inline void AddYaw(float y) { yaw += y; }

	inline void SetMoveWS(float m) { moveWS = m; }
	inline void SetMoveAD(float m) { moveAD = m; }
	inline void SetPitchUpDown(float p) { pitchUpDown = p; }
	inline void SetYawLeftRight(float y) { yawLeftRight = y; }

	inline void SetSpeed(float s) { speed = s; }

	void ResetCamera();

private:
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT2 minMaxPitch;

	float pitch;
	float yaw;


	// INFO: Movement/Rotation Variables (0.0f = No M/R, -1.0f = Negative M/R, 1.0f = Positive M/R)

	float moveWS;
	float moveAD;

	float pitchUpDown;
	float yawLeftRight;

	float speed;
	float rotationSpeed;
};

