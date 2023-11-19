#include "Camera.h"
#include "Window.h"
#include <math.h>
#include "Utility.h"
namespace {
	constexpr const int SPEED_INIT_VAL = 40.f;
	constexpr const float NODE_REACH_THRESHOLD = 10.f;
	constexpr const float SMOOTHNESS_FACTOR = 5.f;
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
				currentNodeToFollowIndex = 0;
				if (this->isAutomaticMovementLoopable)
				{
					_nodeListToFollow = _completedNodeListToFollow;
					_completedNodeListToFollow.clear();
			
				}
				else
				{
					isMovingAutomatically = false;
					_callback();
					return;
				}
			}

			_currentNodeToFollow = _nodeListToFollow[currentNodeToFollowIndex];
		}

		Vector3 direction = (_currentNodeToFollow - _position).Normalised();

		lookAt(direction, dt);
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

void Camera::initAutoMovement(std::vector<Vector3> nodesToFollow, bool isLoopable, std::function<void(void)> callbackAfterNodesFinished) {
	currentNodeToFollowIndex = 0;
	isMovingAutomatically = true;
	_completedNodeListToFollow.clear();
	_nodeListToFollow = nodesToFollow;
	_currentNodeToFollow = _nodeListToFollow[currentNodeToFollowIndex];
	_callback = callbackAfterNodesFinished;
}

void Camera::lookAt(const Vector3& direction, float dt) {

	_pitch = Utility::radiansToDegrees(asin(-direction.y));


	_yaw = Utility::radiansToDegrees(-atan2(direction.x, direction.z));

	//_pitch = Utility::lerp(_pitch, targetPitch, SMOOTHNESS_FACTOR * dt);
	//_yaw = Utility::lerp(_yaw, targetYaw, SMOOTHNESS_FACTOR * dt);


	_pitch = std::min(_pitch, 90.0f);
	_pitch = std::max(_pitch, -90.0f);

	if (_yaw < 0)
		_yaw += 360.0f;
	if (_yaw > 360.0f)
		_yaw -= 360.0f;
}

Vector3 Camera::getDirection() const {
	Vector3 direction = _position;
	direction.Normalise();
	return direction;
}

void Camera::MoveAutomatically(float dt) {

}
