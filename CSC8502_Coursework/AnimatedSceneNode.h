#pragma once

#include "SceneNode.h"

class MeshAnimation;
class MeshMaterial;

class AnimatedSceneNode : public SceneNode {
public:
	AnimatedSceneNode(Mesh* animatedMesh, MeshAnimation* animatedMeshAnimation, MeshMaterial* animatedMeshMaterial);
protected:

	int _currentFrame = 0;
	float _currentFrameTime = 0.f;
	MeshAnimation* _meshAnimation;
	MeshMaterial* _meshMaterial;
	Matrix4 tempModelMatrix;

	vector<GLuint> _materialTextures;

	void setUpShader(OGLRenderer& renderer) override;
	void draw(OGLRenderer& renderer, bool isDrawingForShadow);
	void update(float dt) override;
};