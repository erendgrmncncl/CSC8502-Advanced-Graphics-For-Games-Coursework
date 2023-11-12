#include "Renderer.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Camera.h"
#include "../nclgl/Light.h"
Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	_quad = Mesh::GenerateQuad();
	_heightMap = new HeightMap(TEXTUREDIR"noise.png");

	_waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	_earthTex = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	_earthBump = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	std::vector<std::string> cubePaths = {
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
	};

	
	_cubeMap = LoadCubeMap(cubePaths);

	if (!_earthTex || !_earthBump || !_waterTex || !_cubeMap)
		return;
	SetTextureRepeating(_earthTex, true);
	SetTextureRepeating(_earthBump, true);
	SetTextureRepeating(_waterTex, true);

	_reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	_skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	_lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");

	bool isReflectShaderInitSuccessfully = _reflectShader->LoadSuccess();
	bool isSkyboxShaderInitSuccessfully = _skyboxShader->LoadSuccess();
	bool isLightShaderInitSuccessfully = _lightShader->LoadSuccess();

	if (!isReflectShaderInitSuccessfully || !isSkyboxShaderInitSuccessfully || !isLightShaderInitSuccessfully)
		return;
	Vector3 _heightmapSize = _heightMap->getHeightmapSize();
	_camera = new Camera(-45.f, 0.f, _heightmapSize * Vector3(.5f, 1.5f, .5f));
	const Vector3 lightPos = _heightmapSize * Vector3(.5f, 1.5f, .5f);
	_light = new Light(lightPos, Vector4(1, 1, 1, 1), _heightmapSize.x);
	projMatrix = Matrix4::Perspective(1.f, 15000.f, (float)width / (float)height, 45.f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	waterRotate = .0f;
	waterCycle = .0f;
	init = true;
}

Renderer::~Renderer() {
	delete _camera;
	delete _heightMap;
	delete _quad;
	delete _reflectShader;
	delete _skyboxShader;
	delete _lightShader;
	delete _light;
}

void Renderer::UpdateScene(float dt) {
	_camera->updateCamera(dt);
	viewMatrix = _camera->buildViewMatrix();

	waterRotate += dt * 2.f; //2degree a second.
	waterCycle += dt * .25f; //10 units a second.
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	drawSkybox();
	drawHeightMap();
	drawWater();
}

void Renderer::drawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(_skyboxShader);

	UpdateShaderMatrices();

	_quad->Draw();
	glDepthMask(GL_TRUE);
}

void Renderer::drawHeightMap() {
	BindShader(_lightShader);
	SetShaderLight(*_light);
	glUniform1i(glGetUniformLocation(_lightShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _earthTex);

	glUniform1i(glGetUniformLocation(_lightShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _earthBump);

	modelMatrix.ToIdentity(); //New !
	textureMatrix.ToIdentity(); //New

	UpdateShaderMatrices();

	_heightMap->Draw();
}

void Renderer::drawWater() {
	BindShader(_reflectShader);

	glUniform3fv(glGetUniformLocation(_reflectShader->GetProgram(), "cameraPos"), 1, (float*)&_camera->getPosition());

	glUniform1i(glGetUniformLocation(_reflectShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(_reflectShader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _waterTex);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _cubeMap);

	Vector3 hSize = _heightMap->getHeightmapSize();
	modelMatrix = Matrix4::Translation(hSize * .5f) * Matrix4::Scale(hSize * .5f) * Matrix4::Rotation(90, Vector3(1, 0, 0));
	textureMatrix = Matrix4::Translation(Vector3(waterCycle, .0f, waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(waterRotate, Vector3(0, 0, 1));
	UpdateShaderMatrices();
	_quad->Draw();
}
