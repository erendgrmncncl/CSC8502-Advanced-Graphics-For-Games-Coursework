#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "CubeRobot.h"
#include "SkyboxNode.h"
#include <algorithm>
#include <map>
#include "SkyboxNode.h"
#include "SpaceObject.h"
#include "HeightmapNode.h"
#include "../nclgl/Light.h"
#include "AnimatedSceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"

namespace {

	constexpr const int SHADOW_SIZE = 2048;
	constexpr const int POST_PASSES = 10;

	//TODO(eren.degirmenci): add planet data
	const std::map<std::string, SpaceObjectParams> spaceObjectsDataMap = {
		{"planet_venus", SpaceObjectParams(Vector3(250, 250, 0),Vector3(15, 15, 15), TEXTUREDIR"venus_surface.tga")},
		{"planet_mercury", SpaceObjectParams(Vector3(-40000, 500, -3000),Vector3(750, 750, 750), TEXTUREDIR"mercury_surface.jpg")},
		{"sun", SpaceObjectParams(Vector3(-20000, 0, 0),Vector3(3500, 3500, 3500), TEXTUREDIR"sun_surface.jpg")},
		{"mars", SpaceObjectParams(Vector3(-10000, 300, 1400),Vector3(1200, 1200, 1200), TEXTUREDIR"mars_surface.jpg")}
	};

	const std::vector<std::string> cubePaths = {
	TEXTUREDIR"bkg1_right.png", TEXTUREDIR"bkg1_left.png",
	TEXTUREDIR"bkg1_top.png", TEXTUREDIR"bkg1_bot.png",
	TEXTUREDIR"bkg1_front.png", TEXTUREDIR"bkg1_back.png"
	};

	const std::vector<std::string> planetCubeMapPaths = {
		TEXTUREDIR"rusted_west.jpg", TEXTUREDIR"rusted_east.jpg",
		TEXTUREDIR"rusted_up.jpg", TEXTUREDIR"rusted_down.jpg",
		TEXTUREDIR"rusted_south.jpg", TEXTUREDIR"rusted_north.jpg",
	};

	constexpr const char* SPHERE_MESH_FILE_NAME = "Sphere.msh";
	constexpr const char* TREE_MESH_FILE_NAME = "Tree.msh";

	constexpr const char* SCENE_SHADER_VERTEX_FILE_NAME = "SceneVertex.glsl";
	constexpr const char* SCENE_SHADER_FRAGMENT_FILE_NAME = "SceneFragment.glsl";

	constexpr const char* SCENE_SHADER_FRAGMENT_WITH_SHADOWS = "PerPixelFragment.glsl";
	constexpr const char* SCENE_SHADER_VERTEX_WITH_SHADOWS = "PerPixelVertex.glsl";

	constexpr const char* SKYBOX_SHADER_VERTEX_FILE_NAME = "skyboxVertex.glsl";
	constexpr const char* SKYBOX_SHADER_FRAGMENT_FILE_NAME = "skyboxFragment.glsl";

	constexpr const char* ANIMATED_SHADER_VERTEX_FILE_NAME = "SkinningVertex.glsl";
	constexpr const char* ANIMATED_SHADER_FRAGMENT_FILE_NAME = "texturedFragment.glsl";

	constexpr const char* POST_PROCESS_SHADER_VERTEX_FILE_NAME = "TexturedVertex.glsl";
	constexpr const char* POST_PROCESS_SHADER_FRAGMENT_FILE_NAME = "processfrag.glsl";
}

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	_camera = new Camera(.0f, 90.f, Vector3(812, 341, 2));
	_quad = Mesh::GenerateQuad();
	_sphere = Mesh::LoadFromMeshFile(SPHERE_MESH_FILE_NAME);
	_tree = Mesh::LoadFromMeshFile(TREE_MESH_FILE_NAME);

	_animatedMesh = Mesh::LoadFromMeshFile("Role_T.msh");
	_animateMeshAnimation = new MeshAnimation("Role_T.anm");
	_animatedMeshMaterial = new MeshMaterial("Role_T.mat");

	_shader = new Shader(SCENE_SHADER_VERTEX_FILE_NAME, SCENE_SHADER_FRAGMENT_FILE_NAME);
	_skyboxShader = new Shader(SKYBOX_SHADER_VERTEX_FILE_NAME, SKYBOX_SHADER_FRAGMENT_FILE_NAME);
	_lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	_reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	_shadowShader = new Shader("shadowSceneVertex.glsl", "shadowSceneFragment.glsl");
	_shadowSceneShader = new Shader("shadowSceneVertex.glsl", "shadowSceneFragment.glsl");
	_perPixelSceneShader = new Shader(SCENE_SHADER_VERTEX_WITH_SHADOWS, SCENE_SHADER_FRAGMENT_WITH_SHADOWS);
	_animationShader = new Shader(ANIMATED_SHADER_VERTEX_FILE_NAME, ANIMATED_SHADER_FRAGMENT_FILE_NAME);
	_postProcessShader = new Shader(POST_PROCESS_SHADER_VERTEX_FILE_NAME, POST_PROCESS_SHADER_FRAGMENT_FILE_NAME);
	bool isSkyboxShaderInitSuccessfully = _skyboxShader->LoadSuccess();
	bool isLightShaderInitSuccessfully = _lightShader->LoadSuccess();
	bool isSceneShaderIniSuccessfully = _shader->LoadSuccess();
	bool isReflectShaderInitSuccessfully = _reflectShader->LoadSuccess();
	bool isShadowSceneShaderInitSuccessfully = _shadowSceneShader->LoadSuccess();
	bool isPerPixelSceneShaderInitSuccessfully = _perPixelSceneShader->LoadSuccess();
	bool isAnimationShaderInitSuccessfully = _animationShader->LoadSuccess();
	bool isPostProcessShaderInitSuccessfully = _postProcessShader->LoadSuccess();

	if (!isSceneShaderIniSuccessfully || !isSkyboxShaderInitSuccessfully || !isLightShaderInitSuccessfully 
		|| !isReflectShaderInitSuccessfully || !isShadowSceneShaderInitSuccessfully || !isPerPixelSceneShaderInitSuccessfully 
		|| !isAnimationShaderInitSuccessfully || !isPostProcessShaderInitSuccessfully)
		return;
	_treeTexture = SOIL_load_OGL_texture(TEXTUREDIR"tree_diffuse.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, 0);

	_cubeMap = LoadCubeMap(cubePaths);
	_planetCubemap = LoadCubeMap(planetCubeMapPaths);

	_heightMapTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren Reds.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	_bumpTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	_waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);

	if (!_bumpTexture || !_heightMapTexture || !_waterTex)
		return;

	glGenTextures(1, &_shadowTex);
	glBindTexture(GL_TEXTURE_2D, _shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glBindTexture(GL_TEXTURE_2D, 0);


	glGenFramebuffers(1, &_shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

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
	glGenFramebuffers(1, &_postProcessFBO);

	glBindFramebuffer(GL_FRAMEBUFFER, _bufferFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_TEXTURE_2D, _bufferDepthTex, 0);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _bufferColourTex[0], 0);

	SetTextureRepeating(_heightMapTexture, true);
	SetTextureRepeating(_waterTex, true);
	SetTextureRepeating(_bumpTexture, true);
	InitGlobalSceneNode();
	InitSpaceSceneNodes();
	InitPlanetSceneNodes();
	_currentSceneRoot = _spaceRoot;
	_globalRoot->addChild(_currentSceneRoot);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Renderer::~Renderer() {
	delete _spaceRoot;
	delete _planetRoot;
	delete _quad;
	delete _camera;
	delete _shader;
	glDeleteTextures(1, &_cubeMap);
	glDeleteTextures(1, &_planetCubemap);
}

void Renderer::UpdateScene(float dt) {
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_G)) {
		toggleScene();
	}
	_camera->updateCamera(dt);
	viewMatrix = _camera->buildViewMatrix();
	//viewMatrix = Matrix4::BuildViewMatrix(_camera->getPosition(), _camera->getDirection());

	_globalRoot->update(dt);
}

void Renderer::buildNodeLists(SceneNode* from) {
	bool checkIsInFrustrum = false;
	if (!from->getIsFrustrumCheckable() || !checkIsInFrustrum || _frameFrustum.isInsideFrustum(*from)) {
		Vector3 dir = from->getWorldTransform().GetPositionVector() - _camera->getPosition();
		from->setCameraDistance(Vector3::Dot(dir, dir));
		_nodeList.push_back(from);
	}

	for (vector<SceneNode*>::const_iterator i = from->getChildIteratorStart(); i != from->getChildIteratorEnd(); i++) {
		buildNodeLists(*i);
	}
}

void Renderer::sortNodeLists() {
	std::sort(_transparentNodeList.rbegin(), _transparentNodeList.rend(), SceneNode::compareByCameraDistance);
	std::sort(_nodeList.begin(), _nodeList.end(), SceneNode::compareByCameraDistance);
}

void Renderer::drawNodes(bool isDrawingForShadows) {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	for (const auto& i : _nodeList) {
		if (isDrawingForShadows) {
			modelMatrix = i->getWorldTransform() * Matrix4::Scale(i->getModelScale());
			UpdateShaderMatrices();
		}
		drawNode(i, isDrawingForShadows);
	}
	for (const auto& i : _transparentNodeList) {
		if (isDrawingForShadows) {
			modelMatrix = i->getWorldTransform() * Matrix4::Scale(i->getModelScale());
			UpdateShaderMatrices();
		}
		drawNode(i, isDrawingForShadows);
	}
}

void Renderer::drawNode(SceneNode* node, bool isDrawingForShadows) {
	if (node->getMesh()) {
		node->draw(*this, isDrawingForShadows);
	}
}

void Renderer::InitGlobalSceneNode() {

	_globalRoot = new SceneNode();
	InitSkyboxNode();
	_globalRoot->addChild(_skyboxNode);
}

void Renderer::InitSpaceSceneNodes() {
	_spaceRoot = new SceneNode();

	for (auto planetData : spaceObjectsDataMap) {
		GLuint texture = SOIL_load_OGL_texture(planetData.second._texturePath,
			SOIL_LOAD_AUTO,
			SOIL_CREATE_NEW_ID, 0);
		auto* planet = new SpaceObject(_sphere, texture, _shader, planetData.second);
		planet->setNodeName(planetData.first);
		_spaceRoot->addChild(planet);
	}
}

void Renderer::InitPlanetSceneNodes() {
	_planetRoot = new SceneNode();

	HeightMapNode* heightMapNode = new HeightMapNode(_shadowSceneShader, _camera,
		HeightMapNodeParameter(
			TEXTUREDIR"islandNoise.jpg", _heightMapTexture,
			_bumpTexture, _waterTex, _planetCubemap, _shadowTex, _quad,
			_reflectShader));
	heightMapNode->setBoundingRadius(100.f);
	heightMapNode->setNodeName("heightmap_node");
	_heightMapSize = heightMapNode->getHeightMapSize();

	Vector3 lightPos = _heightMapSize * Vector3(1.1f, 1.1f, 1.1f);
	lightPos.y += 300.f;
	heightMapNode->setLight(new Light(_heightMapSize * Vector3(0.5f, .2f, 0.f), Vector4(1, 1, 1, 1), 400000));
	_currentLight = heightMapNode->getLight();

	for (int i = 0; i < 1; i++) {
		auto* treeNode = new SceneNode(_sphere);
		treeNode->setCamera(_camera);
		treeNode->setLight(_currentLight);

		treeNode->setShader(_shadowSceneShader);
		treeNode->setColour(Vector4(1.0f, 1.0f, 1.0f, 1.0f));

		float modelScale = 10.f * 4;
		treeNode->setModelScale(Vector3(modelScale, modelScale, modelScale));
		float startPosX = _heightMapSize.x * .25f;
		float startPosZ = _heightMapSize.z * .25f;
		int xRange = _heightMapSize.x - startPosX + 1;
		int zRange = _heightMapSize.x - startPosX + 1;
		float xPos = (float)(rand() % (int)xRange + startPosX);
		float zPos = (float)(rand() % (int)zRange + startPosZ);
		float yPos = _heightMapSize.y;

		Vector3 treePos = Vector3(xPos, yPos, zPos);

		treeNode->setTransform(Matrix4::Translation(treePos));
		treeNode->setTexture(_treeTexture);
		treeNode->setShadowTexture(_shadowTex);
		treeNode->setBumpTexture(_bumpTexture);
		treeNode->setBoundingRadius(40.f);
		treeNode->setNodeName("tree_node");
		_planetRoot->addChild(treeNode);
	}

	auto* animatedNode = new AnimatedSceneNode(_animatedMesh, _animateMeshAnimation, _animatedMeshMaterial);
	animatedNode->setShader(_animationShader);
	animatedNode->setModelScale(Vector3(1.f, 1.f, 1.f));
	animatedNode->setBoundingRadius(1.f);
	animatedNode->setNodeName("animated_node");
	animatedNode->setModelScale(Vector3(100, 100, 100));
	animatedNode->setTransform(Matrix4::Translation(Vector3(1300, 155, 1300)));
	_planetRoot->addChild(animatedNode);

	m_planetSceneCameraPos = _heightMapSize * Vector3(.5f, 1.f, .5f);
	m_planetSceneCameraPos.y += 100.f;

	_planetRoot->addChild(heightMapNode);
}

void Renderer::InitSkyboxNode() {
	_skyboxNode = new SkyboxNode();
	_skyboxNode->setMesh(_quad);
	_skyboxNode->setShader(_skyboxShader);
	_skyboxNode->setNodeName("Space_SkyboxNode");
	_skyboxNode->setTexture(_cubeMap);

	_skyboxNode->setIsFrustrumCheckable(false);
}

void Renderer::RenderScene() {

	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	projMatrix = Matrix4::Perspective(1.0f, 50000.f, (float)width / (float)height, 45.0f);
	buildNodeLists(_globalRoot);
	sortNodeLists();
	if (_sceneToggle)
	{
		drawShadowScene();
	}
	drawNodes(false);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (_sceneToggle)
	{
		//drawPostProcess();
		//presentScene();
	}

	clearNodeLists();
}

void Renderer::clearNodeLists() {
	_transparentNodeList.clear();
	_nodeList.clear();
}

void Renderer::toggleScene() {
	_sceneToggle = !_sceneToggle;
	_globalRoot->removeChild(_currentSceneRoot);
	if (_sceneToggle)
		_currentSceneRoot = _planetRoot;
	else
		_currentSceneRoot = _spaceRoot;
	_globalRoot->addChild(_currentSceneRoot);
	onChangeScene();
}

void Renderer::onChangeScene() {
	if (_sceneToggle) {
		_camera->setPosition(m_planetSceneCameraPos);
		_skyboxNode->setTexture(_planetCubemap);
	}
	else {
		_skyboxNode->setTexture(_cubeMap);
	}
}

void Renderer::drawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

	BindShader(_shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(_currentLight->getPosition() * Vector3(1.0f, 1.0f, 1.0f), _heightMapSize * Vector3(0.5f, 0.0f, 0.5f));
	projMatrix = Matrix4::Perspective(1, 100, 1, 45);

	shadowMatrix = projMatrix * viewMatrix;

	drawNodes(true);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glViewport(0, 0, width, height);
	viewMatrix = _camera->buildViewMatrix();
	projMatrix = Matrix4::Perspective(1.0f, 50000.f, (float)width / (float)height, 45.0f);
}

void Renderer::drawPostProcess() {
	glBindFramebuffer(GL_FRAMEBUFFER, _postProcessFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _bufferColourTex[1], 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(_postProcessShader);

	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();

	glDisable(GL_DEPTH_TEST);

	glActiveTexture(GL_TEXTURE0);
	glUniform1i(glGetUniformLocation(_postProcessShader->GetProgram(), "sceneTex"), 0);
	for (int i = 0; i < POST_PASSES; i++) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _bufferColourTex[1], 0);
		glUniform1i(glGetUniformLocation(_postProcessShader->GetProgram(), "isVertical"), 0);
		glBindTexture(GL_TEXTURE_2D, _bufferColourTex[0]);
		_quad->Draw();

		//Swap to colour buffers and do the second blur pass
		glUniform1i(glGetUniformLocation(_postProcessShader->GetProgram(), "isVertical"), 1);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _bufferColourTex[0], 0);
		glBindTexture(GL_TEXTURE_2D, _bufferColourTex[1]);
		_quad->Draw();
	}
}

void Renderer::presentScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(_shadowSceneShader);
	modelMatrix.ToIdentity();
	viewMatrix.ToIdentity();
	projMatrix.ToIdentity();
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(_shadowSceneShader->GetProgram(), "diffuseTex"), 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _bufferColourTex[0]);
	_quad->Draw();
}