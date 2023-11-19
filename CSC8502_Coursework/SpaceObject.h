#pragma once

#include "SceneNode.h"

struct SpaceObjectParams {
	float boundingRadius;
	Vector3 _transform;
	Vector3 _scale;
	Vector4 _color;
	const char* _texturePath;

	SpaceObjectParams(Vector3 transform, Vector3 scale, Vector4 color,const char* texturePath);
};

class SpaceObject : public SceneNode {
public:
	SpaceObject(Mesh* mesh, GLuint texture, Shader* shader, SpaceObjectParams params);
};