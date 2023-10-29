#include "Renderer.h"
#include "../nclgl/Camera.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	_triangle = Mesh::GenerateTriangleWithTexture();

	_texture = SOIL_load_OGL_texture(TEXTUREDIR"brick.tga",
		SOIL_LOAD_AUTO, 
		SOIL_CREATE_NEW_ID, 0);
	_second_texture = SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, 0); 
	if (!_texture || !_second_texture) // Check if texture is created successfully
		return;
	_shader = new Shader("texturedVertex.glsl", "texturedFragment.glsl");
	if (!_shader->LoadSuccess())
		return;
	_isRepeating = true;
	_isFiltering = true;
	_blendRatio = 0.1f;

	init = true;
}

Renderer::~Renderer() {
	delete _triangle;
	delete _shader;
	glDeleteTextures(1, &_texture);
}

void Renderer::SwitchToPerspective() {
	projMatrix = Matrix4::Perspective(1.0f, 10000.0f, float(width) / (float)height, _fov);
}

void Renderer::SwitchToOrthographic() {
	projMatrix = Matrix4::Orthographic(-1.0f, 10000.0f, width / 2.0f, -width / 2.0f, height / 2.0f, -height / 2.0f);
}

void Renderer::UpdateTextureMatrix(float value){
	Matrix4 push = Matrix4::Translation(Vector3(-0.5f, -0.5f, 0));
	Matrix4 pop = Matrix4::Translation(Vector3(0.5f, 0.5f, 0));
	Matrix4 rotation = Matrix4::Rotation(value, Vector3(0, 0, 1));
	textureMatrix = pop * rotation * push;
}

void Renderer::ToggleRepeating(){
	_isRepeating = !_isRepeating;
	SetTextureRepeating(_texture, _isRepeating);
	SetTextureRepeating(_second_texture, _isRepeating);
}

void Renderer::ToggleFiltering(){
	_isFiltering = !_isFiltering;
	glBindTexture(GL_TEXTURE_2D, _texture);
	glBindTexture(GL_TEXTURE_2D, _second_texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, _isFiltering ? GL_LINEAR : GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, _isFiltering ? GL_LINEAR : GL_NEAREST);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::RenderScene() {
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	BindShader(_shader);
	UpdateShaderMatrices();

	glUniform1i(glGetUniformLocation(_shader->GetProgram(),
		"diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(_shader->GetProgram(),
		"secondTex"), 1);
	glUniform1f(glGetUniformLocation(_shader->GetProgram(),
		"blendRatio"), _blendRatio);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _second_texture);


	_triangle->Draw();
}

void Renderer::UpdateScene(float dt) {
	_camera->updateCamera(dt);
	viewMatrix = _camera->buildViewMatrix();
}