#include "CubeRobot.h"

CubeRobot::CubeRobot(Mesh* cube) {
	SceneNode* body = new SceneNode(cube, Vector4(1, 0, 0, 1));
	body->setModelScale(Vector3(10, 15, 5));
	body->setTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	addChild(body);

	_head = new SceneNode(cube, Vector4(0, 1, 0, 1));
	_head->setModelScale(Vector3(5, 5, 5));
	_head->setTransform(Matrix4::Translation(Vector3(0, 30, 0)));
	body->addChild(_head);
	

	_leftArm = new SceneNode(cube, Vector4(0, 1, 0, 1));
	_leftArm->setModelScale(Vector3(3, -18, 3));
	_leftArm->setTransform(Matrix4::Translation(Vector3(-12, 30, -1)));
	body->addChild(_leftArm);

	_rightArm = new SceneNode(cube, Vector4(0, 0, 1, 1)); // Blue !
	_rightArm->setModelScale(Vector3(3, -18, 3));
	_rightArm->setTransform(Matrix4::Translation(Vector3(12, 30, -1)));
	body->addChild(_rightArm);

	SceneNode* leftLeg = new SceneNode(cube, Vector4(0, 0, 1, 1)); // Blue 
	leftLeg->setModelScale(Vector3(3, -17.5, 3));
	leftLeg->setTransform(Matrix4::Translation(Vector3(-8, 0, 0)));
	body->addChild(leftLeg);

	SceneNode* rightLeg = new SceneNode(cube, Vector4(0, 0, 1, 1)); // Blue !
	rightLeg->setModelScale(Vector3(3, -17.5, 3));
	rightLeg->setTransform(Matrix4::Translation(Vector3(8, 0, 0)));
	body->addChild(rightLeg);

	body->setBoundingRadius(15.f);
	_head->setBoundingRadius(5.f);
	_leftArm->setBoundingRadius(18.f);
	_rightArm->setBoundingRadius(18.f);

	leftLeg->setBoundingRadius(18.f);
	rightLeg->setBoundingRadius(18.f);
}

CubeRobot::~CubeRobot()
{
}

void CubeRobot::update(float dt) {
	_transform = _transform * Matrix4::Rotation(30.0f * dt, Vector3(0, 1, 0));
	_head->setTransform(_head->getTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(0, 1, 0)));

	_leftArm->setTransform(_leftArm->getTransform() * Matrix4::Rotation(-30.0f * dt, Vector3(1, 0, 0)));

	_rightArm->setTransform(_rightArm->getTransform() * Matrix4::Rotation(30.0f * dt, Vector3(1, 0, 0)));

	SceneNode::update(dt);
}
