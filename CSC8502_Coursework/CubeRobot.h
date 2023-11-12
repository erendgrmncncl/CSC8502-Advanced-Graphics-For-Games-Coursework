#pragma once
#include "SceneNode.h"

class CubeRobot : public SceneNode {
public:
	CubeRobot(Mesh* cube, Shader* shader = nullptr);
	~CubeRobot();

	void update(float dt) override;

protected:
	SceneNode* _head;
	SceneNode* _leftArm;
	SceneNode* _rightArm;
};