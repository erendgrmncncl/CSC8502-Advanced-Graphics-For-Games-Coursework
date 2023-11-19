#include "Camera.h"
#include "Window.h"
#include <math.h>

namespace {
	constexpr const int SPEED_INIT_VAL = 40.f;
	constexpr const float NODE_REACH_THRESHOLD = 10.f;
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

Camera::Camera(float pitch, float yaw, Vector3 position, std::vector<Vector3> nodeListToFollow) {
	_yaw = yaw;
	_pitch = pitch;
	_position = position;
	_speed = SPEED_INIT_VAL;
	_nodeListToFollow = nodeListToFollow;
	_currentNodeToFollow = _nodeListToFollow[currentNodeToFollowIndex];
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
	else {
		float distanceToNode = (_currentNodeToFollow - _position).Length();
		bool isArrivedToNextNode = distanceToNode < NODE_REACH_THRESHOLD;
		if (isArrivedToNextNode) {
			_completedNodeListToFollow.push_back(_currentNodeToFollow);
			currentNodeToFollowIndex++;

			if (_nodeListToFollow.size() == currentNodeToFollowIndex) {
				_nodeListToFollow = _completedNodeListToFollow;
				_completedNodeListToFollow.clear();
				currentNodeToFollowIndex = 0;
			}

			_currentNodeToFollow = _nodeListToFollow[currentNodeToFollowIndex];
		}

		Vector3 direction = (_currentNodeToFollow - _position).Normalised();
		
		lookAt(direction);
		_position += direction * currentSpeed;
	}

	if (keyboard->KeyDown(KEYBOARD_SHIFT))
		_speed += 2.f;
	if (keyboard->KeyDown(KEYBOARD_C)) {
		if (_speed -= 2.f > 0.f) {
			_speed -= 2.f;
		}
	}
}

void Camera::lookAt(const Vector3& direction){

	// Calculate pitch
	_pitch = RadiansToDegrees(asin(-direction.y));

	// Calculate yaw
	_yaw = RadiansToDegrees(-atan2(direction.x, direction.z));

	// Ensure pitch is within the valid range (-90 to 90 degrees)
	_pitch = std::min(_pitch, 90.0f);
	_pitch = std::max(_pitch, -90.0f);

	// Ensure yaw is within the valid range (0 to 360 degrees)
	if (_yaw < 0)
		_yaw += 360.0f;
	if (_yaw > 360.0f)
		_yaw -= 360.0f;
}

float Camera::RadiansToDegrees(float radians){
	float pi = std::atan(1) * 4;
	return radians * (180.0f / pi);
}

Vector3 Camera::getDirection() const {
	Vector3 direction = _position;
	direction.Normalise();
	return direction;
}

void Camera::MoveAutomatically(float dt) {

}
