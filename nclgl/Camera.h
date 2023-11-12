#pragma once
#include "./Matrix4.h"
#include "./Vector3.h"
class Camera {
public:
	Camera();
	Camera(float pitch, float yaw, Vector3 position);
	~Camera();

	inline float getYaw() const { return _yaw; }
	inline float getPitch() const { return _pitch; }

	Matrix4 buildViewMatrix();
	inline Vector3 getPosition() { return _position; }

	void updateCamera(float dt = 1.0f);
	inline void setYaw(float yaw) { _yaw = yaw; }
	inline void setPitch(float pitch) { _pitch = pitch; }
	inline void setPosition(Vector3 position) { _position = position; }
protected:
	Vector3 _position;
	float _yaw;
	float _pitch;
	float _speed;
};