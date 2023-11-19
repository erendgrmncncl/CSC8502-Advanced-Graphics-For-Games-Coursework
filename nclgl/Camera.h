#pragma once
#include "./Matrix4.h"
#include "./Vector3.h"
#include <vector>
#include <functional>

class Camera {
public:
	Camera();
	Camera(float pitch, float yaw, Vector3 position);
	~Camera();

	inline float getYaw() const { return _yaw; }
	inline float getPitch() const { return _pitch; }

	Matrix4 buildViewMatrix();
	inline Vector3 getPosition() { return _position; }
	Vector3 getDirection() const;

	void updateCamera(float dt = 1.0f);


	inline void setYaw(float yaw) { _yaw = yaw; }
	inline void setPitch(float pitch) { _pitch = pitch; }
	inline void setPosition(Vector3 position) { _position = position; }

	void initAutoMovement(std::vector<Vector3> nodesToFollow, bool isLoopable, std::function<void(void)> callbackAfterNodesFinished = nullptr);
	
	Vector3 _currentNodeToFollow;
	std::vector<Vector3> _nodeListToFollow;
	std::vector<Vector3> _completedNodeListToFollow;

	std::vector<Vector3> _nodeToLookAt;
	std::vector<Vector3> _completedNodesToLookAt;

protected:
	int currentNodeToFollowIndex = 0;
	bool isMovingAutomatically = false;

	bool isAutomaticMovementLoopable = false;
	std::function<void(void)> _callback = nullptr;
	
	Vector3 _position;
	float _yaw;
	float _pitch;
	float _speed;

	void MoveAutomatically(float dt);
		void lookAt(const Vector3& target, float dt);
};