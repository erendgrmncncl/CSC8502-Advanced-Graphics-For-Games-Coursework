#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
#include "../nclgl/Shader.h"
#include "../nclgl/HeightMap.h"
#include "Renderer.h"

Renderer::Renderer(Window& window) : OGLRenderer(window) {
	_heightMap = new HeightMap(TEXTUREDIR"noise.png");
	_texture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	_shader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");

	if (!_shader->LoadSuccess() || !_texture)
		return;
	SetTextureRepeating(_texture, true);

	Vector3 heightMapSize = _heightMap->getHeightmapSize();
	_camera = new Camera(-45.f, 0.f, heightMapSize * Vector3(.5f, 5.f, 0.5f));

	for (int i = 0; i < 4; i++){
		auto* light = new Light(heightMapSize * Vector3(.5f + ((i+1) * 5) , 1.5f, .5f), Vector4( .2f * (i+1), 1, 1, 1), heightMapSize.x * .5f, Vector4(0, 1, 0, 1.f));
		_lights.push_back(light);
	}

	projMatrix = Matrix4::Perspective(1.f, 15000.f, (float)width / (float)height, 45.f);

	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer::~Renderer() {
	delete _camera;
	delete _heightMap;
	delete _shader;
}

void Renderer::UpdateScene(float dt) {
	_camera->updateCamera(dt);
	viewMatrix = _camera->buildViewMatrix();
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(_shader);

	glUniform1i(glGetUniformLocation(_shader->GetProgram(), "diffuseTex"), 0);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glUniform3fv(glGetUniformLocation(_shader->GetProgram(), "cameraPos"), 1, (float*)&_camera->getPosition());

	UpdateShaderMatrices();
	for (const auto* light : _lights) {
		SetShaderLight(*light);
	}
	
	_heightMap->Draw();
}