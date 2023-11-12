#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/HeightMap.h"

namespace {
	constexpr const int POST_PASSES = 10;
}

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	_camera = new Camera(-25.f, 225.f, Vector3(-150.f, 250.f, -150.f));
	_quad = Mesh::GenerateQuad();

	_heightMap = new HeightMap(TEXTUREDIR"noise.png");
	_heightMapTexture = SOIL_load_OGL_texture(TEXTUREDIR"rusted_down.jpg", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	_sceneShader = new Shader("TexturedVertex.glsl", "TexturedFragment.glsl");
	_processShader = new Shader("TexturedVertex.glsl", "processfrag.glsl");
	bool isSceneShaderLoadedSuccessfully = _sceneShader->LoadSuccess();
	bool isProcessShaderLoadedSuccessfully = _processShader->LoadSuccess();
	if (!isSceneShaderLoadedSuccessfully || !isProcessShaderLoadedSuccessfully || !_heightMapTexture)
		return;
	SetTextureRepeating(_heightMapTexture, true);
	//Generate scene depth texture
	glGenTextures(1, &_bufferDepthTex);
	glBindTexture(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, width, height, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);

	//ColourTexture
	for (int i = 0; i < 2; ++i) {
		glGenTextures(1, &_bufferColourTex[i]);
		glBindTexture(GL_TEXTURE_2D, _bufferColourTex[i]);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	}
	glGenFramebuffers(1, &_bufferFBO);
	glGenFramebuffers(1, &_processFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, _bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _bufferColourTex[0], 0);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE || !_bufferDepthTex || !_bufferColourTex[0])
		return;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer::~Renderer() {
	delete _sceneShader;
	delete _processShader;
	delete _heightMap;
	delete _quad;
	delete _camera;

	glDeleteTextures(2, _bufferColourTex);
	glDeleteTextures(1, &_bufferDepthTex);
	glDeleteFramebuffers(1, &_bufferFBO);
	glDeleteFramebuffers(1, &_processFBO);
}

void Renderer::UpdateScene(float dt) {
	_camera->updateCamera(dt);
	viewMatrix = _camera->buildViewMatrix();
}

void Renderer::RenderScene() {
	drawScene();
	drawPostProcess();
	presentScene();
}

void Renderer::drawScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, _bufferFBO);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	BindShader(_sceneShader);
	projMatrix = Matrix4::Perspective(1.f, 10000.f, (float)width / (float)height, 45.f);

	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(_sceneShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _heightMapTexture);
	_heightMap->Draw();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::drawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, _processFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(_processShader);

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(_processShader->GetProgram(), "sceneTex"), 0);
	for (int i = 0; i < POST_PASSES; i++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(_processShader->GetProgram(), "isVertical"), 0);
		glBindTexture(GL_TEXTURE_2D, _bufferColourTex[0]);
		_quad->Draw();
		//Swap to colour buffers and do the second blur pass
		glUniform1i(glGetUniformLocation(_processShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, _bufferColourTex[1]);
		_quad->Draw();
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::presentScene()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(_sceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _bufferColourTex[0]);
	glUniform1i(glGetUniformLocation(_sceneShader->GetProgram(), "diffuseTex"), 0);
	_quad->Draw();
}
