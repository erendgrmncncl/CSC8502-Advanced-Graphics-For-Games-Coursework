#pragma once

#include "../nclgl/OGLRenderer.h"

class Camera;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& window);
	~Renderer();

	void UpdateScene(float dt) override;
	void RenderScene() override;


protected:
	float _sceneTime;

	GLuint _shadowTex;
	GLuint shadowFBO;
	GLuint _sceneDiffuse;
	GLuint _sceneBump;
	
	Shader* _sceneShader;
	Shader* _shadowShader;
	
	Camera* _camera;
	Light* _light;

	std::vector<Mesh*> _sceneMeshes;
	std::vector<Matrix4> _sceneTransforms;

	void drawShadowScene();
	void drawMainScene();
	
};