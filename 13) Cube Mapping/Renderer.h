#pragma once

#include "../nclgl/OGLRenderer.h"

class Camera;
class Shader;
class HeightMap;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer();
	void RenderScene() override;
	void UpdateScene(float dt) override;
protected:
	void drawHeightMap();
	void drawWater();
	void drawSkybox();

	Shader* _lightShader;
	Shader* _reflectShader;
	Shader* _skyboxShader;

	HeightMap* _heightMap;
	Mesh* _quad;

	Light* _light;
	Camera* _camera;

	GLuint _cubeMap;
	GLuint _waterTex;
	GLuint _earthTex;
	GLuint _earthBump;

	float waterRotate;
	float waterCycle;

};