#include "Camera.h"
#include "Window.h"
#include <algorithm>

namespace {
	constexpr const int SPEED_INIT_VAL = 40.f;
}

Camera::Camera() {
	_yaw = .0f;
	_pitch = .0f;
	_speed = SPEED_INIT_VAL;
}

Camera::Camera(float pitch, float yaw, Vector3 position) {
	_yaw = yaw;
	_pitch = pitch;
	_position = position;
	_speed = SPEED_INIT_VAL;
}

Camera::~Camera() {

}

Matrix4 Camera::buildViewMatrix() {
	return Matrix4::Rotation(-_pitch, Vector3(1, 0, 0)) *
		Matrix4::Rotation(-_yaw, Vector3(0, 1, 0)) *
		Matrix4::Translation(-_position);
}
	
void Camera::updateCamera(float dt) {
	Vector2 mousePos = Window::GetMouse()->GetRelativePosition();
	_pitch -= (mousePos.y);
	_yaw -= (mousePos.x);
	if (mousePos.x != 0)
	{
		int a = 0;
	}

	_pitch = std::min(_pitch, 90.0f);
	_pitch = std::max(_pitch, -90.0f);

	if (_yaw < 0)
		_yaw += 360.0f;
	if (_yaw > 360.0f)
		_yaw -= 360.0f;

	Matrix4 rotation = Matrix4::Rotation(_yaw, Vector3(0, 1, 0));
	Vector3 forward = rotation * Vector3(0, 0, 1);
	Vector3 right = rotation * Vector3(1, 0, 0);

	float currentSpeed = _speed * dt;
	auto* keyboard = Window::GetKeyboard();
	if (keyboard->KeyTriggered(KEYBOARD_T))
		isMovingAutomatically = !isMovingAutomatically;
	if (!isMovingAutomatically) {
		if (keyboard->KeyDown(KEYBOARD_W))
			_position -= forward * currentSpeed;
		if (keyboard->KeyDown(KEYBOARD_S))
			_position += forward * currentSpeed;
		if (keyboard->KeyDown(KEYBOARD_A))
			_position -= right * currentSpeed;
		if (keyboard->KeyDown(KEYBOARD_D))
			_position += right * currentSpeed;
		if (keyboard->KeyDown(KEYBOARD_CONTROL))
			_position.y -= currentSpeed;
		if (keyboard->KeyDown(KEYBOARD_SPACE))
			_position.y += currentSpeed;
	}
	else{
		_position -= forward * currentSpeed;
	}
	
	if (keyboard->KeyDown(KEYBOARD_SHIFT))
		_speed += 2.f;
	if (keyboard->KeyDown(KEYBOARD_C)) {
		if (_speed -= 2.f > 0.f) {
			_speed -= 2.f;
		}

	}
}

void Camera::MoveAutomatically(float dt){
	
}
