#pragma once
#include "../nclgl/OGLRenderer.h"
class HeightMap;
class Camera;
class Light;
class Shader;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer();

	void RenderScene() override;
	void UpdateScene(float dt) override;

protected:
	HeightMap* _heightMap;
	Shader* _shader;
	Camera* _camera;
	Light* _light;
	GLuint _texture;
	GLuint _bumpmap;
};