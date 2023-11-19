#pragma once

#include "../nclgl/Vector3.h"
#include "./SceneNode.h"

class Mesh;

struct ParticleStruct {
	bool _isOnScene;
	Vector3 _position;
	Vector3 _speed;
};


class ParticleNode : public SceneNode {
public:
	ParticleNode(Mesh* mesh, GLuint particleTexture);
	void update(float dt) override;
	void draw(OGLRenderer& renderer, bool isDrawingForShadows) override;
protected:
	int _particleCount = 0;

	std::vector<ParticleStruct> _particles;
};