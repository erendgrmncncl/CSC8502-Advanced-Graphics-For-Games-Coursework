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
namespace {

	constexpr const int SHADOW_SIZE = 2048;
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

	constexpr const char* SKYBOX_SHADER_VERTEX_FILE_NAME = "skyboxVertex.glsl";
	constexpr const char* SKYBOX_SHADER_FRAGMENT_FILE_NAME = "skyboxFragment.glsl";
}

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	_camera = new Camera(.0f, 90.f, Vector3(812, 341, 2));
	_quad = Mesh::GenerateQuad();
	_sphere = Mesh::LoadFromMeshFile(SPHERE_MESH_FILE_NAME);
	_tree = Mesh::LoadFromMeshFile(TREE_MESH_FILE_NAME);

	_shader = new Shader(SCENE_SHADER_VERTEX_FILE_NAME, SCENE_SHADER_FRAGMENT_FILE_NAME);
	_skyboxShader = new Shader(SKYBOX_SHADER_VERTEX_FILE_NAME, SKYBOX_SHADER_FRAGMENT_FILE_NAME);
	_lightShader = new Shader("PerPixelVertex.glsl", "PerPixelFragment.glsl");
	_reflectShader = new Shader("reflectVertex.glsl", "reflectFragment.glsl");
	_shadowShader = new Shader("shadowSceneVertex.glsl", "shadowSceneFragment.glsl");
	bool isSkyboxShaderInitSuccessfully = _skyboxShader->LoadSuccess();
	bool isLightShaderInitSuccessfully = _lightShader->LoadSuccess();
	bool isSceneShaderIniSuccessfully = _shader->LoadSuccess();
	bool isReflectShaderInitSuccessfully = _reflectShader->LoadSuccess();

	if (!isSceneShaderIniSuccessfully || !isSkyboxShaderInitSuccessfully || !isLightShaderInitSuccessfully || !isReflectShaderInitSuccessfully)
		return;
	_treeTexture = SOIL_load_OGL_texture(TEXTUREDIR"tree_diffuse.png",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, 0);

	_cubeMap = LoadCubeMap(cubePaths);
	_planetCubemap = LoadCubeMap(planetCubeMapPaths);

	_heightMapTexture = SOIL_load_OGL_texture(TEXTUREDIR"Grass.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	_bumpTexture = SOIL_load_OGL_texture(TEXTUREDIR"Barren RedsDOT3.JPG", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	_waterTex = SOIL_load_OGL_texture(TEXTUREDIR"water.TGA", SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	
	if (!_bumpTexture || !_heightMapTexture)
		return;

	glGenTextures(1, &_shadowTex);
	glBindTexture(GL_TEXTURE_2D, _shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_SIZE, SHADOW_SIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &_shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBO);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	SetTextureRepeating(_heightMapTexture, true);
	InitGlobalSceneNode();
	InitSpaceSceneNodes();
	InitPlanetSceneNodes();
	_currentSceneRoot = _spaceRoot;
	_globalRoot->addChild(_currentSceneRoot);


	projMatrix = Matrix4::Perspective(1.0f, 500000.f, (float)width / (float)height, 45.f);

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
	for (const auto& i : _nodeList) {
		drawNode(i, isDrawingForShadows);
	}
	for (const auto& i : _transparentNodeList) {
		drawNode(i, isDrawingForShadows);
	}
}

void Renderer::drawNode(SceneNode* node, bool isDrawingForShadows) {
	if (node->getMesh()) {
		node->draw(*this, isDrawingForShadows);
	}
}

void Renderer::InitGlobalSceneNode(){

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

	HeightMapNode* heightMapNode = new HeightMapNode(_lightShader, _camera, HeightMapNodeParameter(TEXTUREDIR"noise.png", _heightMapTexture, _bumpTexture, _waterTex, _planetCubemap, _quad, _reflectShader));
	heightMapNode->setBoundingRadius(100.f);
	heightMapNode->setNodeName("heightmap_node");

	Vector3 hMapSize = heightMapNode->getHeightMapSize();

	Vector3 lightPos = hMapSize * Vector3(1.1f, 1.1f, 1.1f);
	lightPos.y += 300.f;
	heightMapNode->setLight(new Light(hMapSize * Vector3(.5f, 5.5f, 0.5f), Vector4(1, 1, 1, 1), hMapSize.x * .5f));
	_currentLight = heightMapNode->getLight();

	for (int i = 0; i < 50; i++){
		auto* treeNode = new SceneNode(_tree);
		treeNode->setShader(_shader);
		float modelScale = 10.f * (i % 10 + 4);
		treeNode->setModelScale(Vector3(modelScale, modelScale, modelScale));
		float startPosX = hMapSize.x * .25f;
		float startPosZ = hMapSize.z * .25f;
		int xRange = hMapSize.x - startPosX + 1;
		int zRange = hMapSize.x - startPosX + 1;
		float xPos = (float)(rand() % (int)xRange + startPosX);
		float zPos = (float)(rand() % (int)zRange + startPosZ);
		float yPos = hMapSize.y;
		
		Vector3 treePos = Vector3(xPos, yPos, zPos);
		treeNode->setTransform(Matrix4::Translation(treePos));
		treeNode->setTexture(_treeTexture);
		treeNode->setBoundingRadius(1.f);
		treeNode->setNodeName("tree_node");
		_planetRoot->addChild(treeNode);
	}

	m_planetSceneCameraPos = hMapSize * Vector3(.5f, 1.f, .5f);
	m_planetSceneCameraPos.y += 100.f;

	_planetRoot->addChild(heightMapNode);
}

void Renderer::InitSkyboxNode(){
	_skyboxNode = new SkyboxNode();
	_skyboxNode->setMesh(_quad);
	_skyboxNode->setShader(_skyboxShader);
	_skyboxNode->setNodeName("Space_SkyboxNode");
	_skyboxNode->setTexture(_cubeMap);

	_skyboxNode->setIsFrustrumCheckable(false);
}

void Renderer::RenderScene() {
	buildNodeLists(_globalRoot);
	sortNodeLists();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	//drawShadowScene();
	drawNodes(false);
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
	if (_sceneToggle){
		_camera->setPosition(m_planetSceneCameraPos);
		_skyboxNode->setTexture(_planetCubemap);
	}
	else{
		_skyboxNode->setTexture(_cubeMap);
	}
}

void Renderer::drawShadowScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, _shadowFBO);

	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, SHADOW_SIZE, SHADOW_SIZE); //Increase size of our virtual window
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE); //disable color write

	BindShader(_shadowShader);

	viewMatrix = Matrix4::BuildViewMatrix(_currentLight->getPosition(),
		Vector3(0, 0, 0));
	projMatrix = Matrix4::Perspective(1, 100, 1, 45);
	shadowMatrix = projMatrix * viewMatrix;
	
	drawNodes(true);

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE); //enable color write
	glViewport(0, 0, width, height); //Increase size of our virtual window

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}