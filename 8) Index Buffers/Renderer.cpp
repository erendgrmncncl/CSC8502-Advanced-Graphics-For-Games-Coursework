#pragma once
#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {

	std::string s = TEXTUREDIR"noise.png";
	_heightMap = new HeightMap(s);
	_camera = new Camera(-40, 270, Vector3());
	Vector3 dimensions = _heightMap->getHeightmapSize();
	_camera->setPosition(dimensions * Vector3(0.5, 2, 0.5));
	_shader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	if (!_shader->LoadSuccess())
		return;
	terrainTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!terrainTex)
		return;
	SetTextureRepeating(terrainTex, true);

	projMatrix = Matrix4::Perspective(1.f, 10000.f, (float)width / (float)height, 45.f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	init = true;
}

Renderer::~Renderer() {
	delete _heightMap;
	delete _camera;
	delete _shader;
}

void Renderer::UpdateScene(float dt) {
	_camera->updateCamera(dt);
	viewMatrix = _camera->buildViewMatrix();
}

void Renderer::RenderScene() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	BindShader(_shader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(_shader->GetProgram(), "diffuseText"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, terrainTex);
	_heightMap->Draw();
}