#pragma once
#include "../nclgl/OGLRenderer.h"

class Camera;
class MeshAnimation;
class MeshMaterial;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer();

	void RenderScene() override;
	void UpdateScene(float dt) override;
protected:
	Camera* _camera;
	Mesh* _mesh;
	Shader* _shader;
	MeshAnimation* _anim;
	MeshMaterial* _material;
	vector<GLuint> _matTextures;

	int _currentFrame;
	float _frameTime;
};