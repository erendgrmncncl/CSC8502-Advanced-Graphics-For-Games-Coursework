#pragma once

#include "../nclgl/OGLRenderer.h"

class  Camera;
class HeightMap;
class Renderer : public OGLRenderer {
public:
	Renderer(Window& window);
	~Renderer();

	void RenderScene() override;
	void UpdateScene(float dt) override;
protected:
	void presentScene();
	void drawPostProcess();
	void drawScene();

	Shader* _sceneShader;
	Shader* _processShader;

	Camera* _camera;

	Mesh* _quad;
	HeightMap* _heightMap;
	GLuint _heightMapTexture;
	GLuint _bufferFBO;
	GLuint _processFBO;
	GLuint _bufferColourTex[2];
	GLuint _bufferDepthTex;
};