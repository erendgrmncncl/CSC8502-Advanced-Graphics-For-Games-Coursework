#include "ParticleNode.h"

namespace {

	constexpr const int PARTICLE_COUNT = 300.f;
}

ParticleNode::ParticleNode(Mesh* mesh, GLuint particleTexture) : SceneNode(mesh){
	_texture = particleTexture;
	_particleCount = PARTICLE_COUNT;
	for (int i = 0; i < _particleCount; i++) {
		ParticleStruct particle;
		particle._isOnScene = true;
		particle._position = Vector3(rand() % 10000, 8000, rand() % 10000);
		float speed = (rand() % 30) + 8;
		particle._speed = Vector3(0, -speed, -3);
		_particles.push_back(particle);
		SceneNode* rain_drop = new SceneNode();
		rain_drop->setMesh(_mesh);
		rain_drop->setTexture(_texture);
		rain_drop->setTransform(Matrix4::Translation(particle._position));
		rain_drop->setModelScale(Vector3(20, 20, 20));
		rain_drop->setColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		rain_drop->setBoundingRadius(5.0f);
		addChild(rain_drop);
	}
}

void ParticleNode::update(float dt){
	for (int i = 0; i < _particleCount; i++) {
		if (_children[i]->getTransform().GetPositionVector().y <= 0) {
			_children[i]->setTransform(Matrix4::Translation(Vector3(rand() % 10000, 8000, rand() % 10000)));
			float speed = (rand() % 30) + 8;
			_particles[i]._speed = Vector3(0, -speed, 0);
		}
		_children[i]->setTransform(_children[i]->getTransform() * Matrix4::Translation(_particles[i]._speed));
	}
	SceneNode::update(dt);
}

void ParticleNode::draw(OGLRenderer& renderer, bool isDrawingForShadows){
	if (isDrawingForShadows)
	{
		return;
	}
	SceneNode::draw(renderer, isDrawingForShadows);
}
