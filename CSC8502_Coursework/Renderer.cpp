#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "CubeRobot.h"
#include "SkyboxNode.h"
#include <algorithm>
#include <map>
#include "SkyboxNode.h"
#include "SpaceObject.h"
#include "HeightmapNode.h"

namespace {
	//TODO(eren.degirmenci): add planet data
	const std::map<std::string, SpaceObjectParams> spaceObjectsDataMap = {
		{"planet_mercury", SpaceObjectParams(Vector3(2, 35, 0),Vector3(15, 15, 15))},
		{"planet_mercury_2", SpaceObjectParams(Vector3(0, 500, 0),Vector3(150, 150, 150))},
		{"sun", SpaceObjectParams(Vector3(-4000, 0, 0),Vector3(1500, 1500, 1500))}
	};

	const std::vector<std::string> cubePaths = {
	TEXTUREDIR"bkg1_right.png", TEXTUREDIR"bkg1_left.png",
	TEXTUREDIR"bkg1_top.png", TEXTUREDIR"bkg1_bot.png",
	TEXTUREDIR"bkg1_front.png", TEXTUREDIR"bkg1_back.png"
	};

	const std::vector<std::string> planetCubeMapPaths = {
		TEXTUREDIR"interstellar_lf.jpg", TEXTUREDIR"interstellar_rt.jpg",
		TEXTUREDIR"interstellar_up.jpg", TEXTUREDIR"interstellar_dn.jpg",
		TEXTUREDIR"interstellar_bk.jpg", TEXTUREDIR"interstellar_ft.jpg"

	};

	constexpr const char* SPHERE_MESH_FILE_NAME = "Sphere.msh";
	constexpr const char* SCENE_SHADER_VERTEX_FILE_NAME = "SceneVertex.glsl";
	constexpr const char* SCENE_SHADER_FRAGMENT_FILE_NAME = "SceneFragment.glsl";

	constexpr const char* SKYBOX_SHADER_VERTEX_FILE_NAME = "skyboxVertex.glsl";
	constexpr const char* SKYBOX_SHADER_FRAGMENT_FILE_NAME = "skyboxFragment.glsl";

	constexpr const char* TEXTURE_VENUS_SURFACE_FILENAME = "venus_surface.tga";
}

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	_camera = new Camera(.0f, .0f, Vector3(0, 100, 750.f));
	_quad = Mesh::GenerateQuad();
	_sphere = Mesh::LoadFromMeshFile(SPHERE_MESH_FILE_NAME);

	_shader = new Shader(SCENE_SHADER_VERTEX_FILE_NAME, SCENE_SHADER_FRAGMENT_FILE_NAME);
	_skyboxShader = new Shader(SKYBOX_SHADER_VERTEX_FILE_NAME, SKYBOX_SHADER_FRAGMENT_FILE_NAME);
	bool isSkyboxShaderInitSuccessfully = _skyboxShader->LoadSuccess();

	_texture = SOIL_load_OGL_texture(TEXTUREDIR"venus_surface.tga",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, 0);

	if (!_shader->LoadSuccess() || !_skyboxShader->LoadSuccess())
		return;

	_cubeMap = LoadCubeMap(cubePaths);
	_planetCubemap = LoadCubeMap(planetCubeMapPaths);

	if (!_texture)
		return;
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
	glDeleteTextures(1, &_texture);
	glDeleteTextures(1, &_cubeMap);
	glDeleteTextures(1, &_planetCubemap);
}

void Renderer::UpdateScene(float dt) {
	if (Window::GetKeyboard()->KeyTriggered(KEYBOARD_G)) {
		toggleScene();
	}
	_camera->updateCamera(dt);
	viewMatrix = _camera->buildViewMatrix();

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

void Renderer::drawNodes() {
	for (const auto& i : _nodeList) {
		drawNode(i);
	}
	for (const auto& i : _transparentNodeList) {
		drawNode(i);
	}
}

void Renderer::drawNode(SceneNode* node) {
	if (node->getMesh()) {
		node->draw(*this);
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
		auto* planet = new SpaceObject(_sphere, _texture, _shader, planetData.second);
		planet->setNodeName(planetData.first);
		_spaceRoot->addChild(planet);
	}
}

void Renderer::InitPlanetSceneNodes() {
	_planetRoot = new SceneNode();

	HeightMapNode* heightMapNode = new HeightMapNode(_shader, HeightMapNodeParameter(TEXTUREDIR"noise.png", TEXTUREDIR"Barren Reds.JPG", TEXTUREDIR"Barren RedsDOT3.JPG"));
	heightMapNode->setBoundingRadius(20.f);
	heightMapNode->setNodeName("heightmap_node");
	Vector3 hMapSize = heightMapNode->getHeightMapSize();
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
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	drawNodes();
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
