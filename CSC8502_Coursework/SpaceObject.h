#pragma once

#include "SceneNode.h"

struct SpaceObjectParams {
	float boundingRadius;
	Vector3 _transform;
	Vector3 _scale;

	SpaceObjectParams(Vector3 transform, Vector3 scale);
};

class SpaceObject : public SceneNode {
public:
	SpaceObject(Mesh* mesh, GLuint texture, Shader* shader, SpaceObjectParams params);
};