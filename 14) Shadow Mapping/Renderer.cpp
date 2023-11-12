#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"

namespace {
	constexpr const int SHADOW_SIZE = 2048;
}

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	_camera = new Camera(-30.f, 315.f, Vector3(-8.f, 5.f, 8.f));
	_light = new Light(Vector3(-20.f, 10.f, -20.f), Vector4(1, 1, 1, 1), 250.f);

	_sceneShader = new Shader("shadowSceneVertex.glsl", "shadowSceneFragment.glsl");
	_shadowShader = new Shader("shadowVertex.glsl", "shadowFragment.glsl");

	bool isSceneShaderLoadedSuccessfully = _sceneShader->LoadSuccess();
	bool isShadowShaderLoadedSuccessfully = _shadowShader->LoadSuccess();

	if (!isSceneShaderLoadedSuccessfully || !isShadowShaderLoadedSuccessfully)
		return;

	glGenTextures(1, &_shadowTex);
	glBindTexture(GL_TEXTURE_2D, _shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	_sceneMeshes.emplace_back(Mesh::GenerateQuad());
	_sceneMeshes.emplace_back(Mesh::LoadFromMeshFile("Sphere.msh"));
	_sceneMeshes.emplace_back(Mesh::LoadFromMeshFile("Cylinder.msh"));
	_sceneMeshes.emplace_back(Mesh::LoadFromMeshFile("Cone.msh"));

	_sceneDiffuse = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	_sceneBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	SetTextureRepeating(_sceneDiffuse, true);
	SetTextureRepeating(_sceneBump, true);

	glEnable(GL_DEPTH_TEST);
	_sceneTransforms.resize(4);
	_sceneTransforms[0] = Matrix4::Rotation(90, Vector3(1, 0, 0)) * Matrix4::Scale(Vector3(10, 10, 1));
	_sceneTime = 0.0f;
	init = true;

}
Renderer::~Renderer() {
	delete _camera;
	for (auto& sceneTransform : _sceneMeshes) {
		delete sceneTransform;
	}

	delete _camera;
	delete _sceneShader;
	delete _shadowShader;
}

void Renderer::UpdateScene(float dt) {
	_camera->updateCamera(dt);
	_sceneTime += dt;

	for (int i = 1; i < 4; ++i)
	{
		Vector3 t = Vector3(-10 + (5 * i), 2.f + sin(_sceneTime * i), 0);
		_sceneTransforms[i] = Matrix4::Translation(t) * Matrix4::Rotation(_sceneTime * 10 * i, Vector3(1, 0, 0));
	}
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	drawShadowScene();
	drawMainScene();
}

void Renderer::drawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	BindShader(_shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(_light->getPosition(), Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(1, 100, 1, 45);
	shadowMatrix = projMatrix * viewMatrix;
	for (int i = 0; i < 4; ++i){
		modelMatrix = _sceneTransforms[i];
		UpdateShaderMatrices();
		_sceneMeshes[i]->Draw();
	}
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::drawMainScene(){
	BindShader(_sceneShader);
	SetShaderLight(*_light);
	viewMatrix = Matrix4::Perspective(1.0f, 15000.f, (float)width / (float)height, 45.f);

	glUniform1i(glGetUniformLocation(_sceneShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(_sceneShader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(_sceneShader->GetProgram(), "shadowTex"), 2);

	glUniform3fv(glGetUniformLocation(_sceneShader->GetProgram(), "cameraPos"), 1,(float*)&_camera->getPosition());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _sceneDiffuse);
	
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _sceneBump);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _shadowTex);

	for (int i = 0; i < 4; ++i){
		modelMatrix = _sceneTransforms[i];
		UpdateShaderMatrices();
		_sceneMeshes[i]->Draw();
	}
}
