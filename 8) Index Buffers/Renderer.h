#pragma once

#include "../nclgl/OGLRenderer.h"

class HeightMap;
class Camera;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer();
	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	HeightMap* _heightMap = nullptr;
	Camera* _camera;
	GLuint terrainTex;
	Shader* _shader;
};