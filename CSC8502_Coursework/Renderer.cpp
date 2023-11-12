#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "CubeRobot.h"
#include "SkyboxNode.h"
#include <algorithm>
#include "SkyboxNode.h"
#include "SpaceObject.h"

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	_camera = new Camera(.0f, .0f, Vector3(0, 100, 750.f));
	_quad = Mesh::GenerateQuad();
	_cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
	_sphere = Mesh::LoadFromMeshFile("Sphere.msh");

	_shader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	_skyboxShader = new Shader("skyboxVertex.glsl", "skyboxFragment.glsl");
	bool isSkyboxShaderInitSuccessfully = _skyboxShader->LoadSuccess();

	_texture = SOIL_load_OGL_texture(TEXTUREDIR"venus_surface.tga",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, 0);

	if (!_shader->LoadSuccess() || !_skyboxShader->LoadSuccess())
		return;

	std::vector<std::string> cubePaths = {
	TEXTUREDIR"bkg1_right.png", TEXTUREDIR"bkg1_left.png",
	TEXTUREDIR"bkg1_top.png", TEXTUREDIR"bkg1_bot.png",
	TEXTUREDIR"bkg1_front.png", TEXTUREDIR"bkg1_back.png"
	};

	_skyboxCubeMap = LoadCubeMap(cubePaths);

	if (!_texture || !_skyboxCubeMap)
		return;

	_root = new SceneNode();

	//for (int i = 0; i < 5; i++) {
	//	SceneNode* node = new SceneNode();
	//	node->setColour(Vector4(1.f, 1.f, 1.f, .5f));
	//	node->setTransform(Matrix4::Translation(Vector3(.0f, 100.f, -300.f + 100.f + 100 * i)));
	//	node->setModelScale(Vector3(100.f, 100.f, 100.f));
	//	node->setBoundingRadius(100.f);
	//	node->setMesh(_quad);
	//	node->setTexture(_texture);
	//	_root->addChild(node);
	//}

	
	SkyboxNode* skyboxNode = new SkyboxNode();
	skyboxNode->setMesh(_quad);
	skyboxNode->setShader(_skyboxShader);
	skyboxNode->setNodeName("SkyboxNode");
	skyboxNode->setIsFrustrumCheckable(false);

	auto* planet = new SpaceObject(_sphere, _texture, _shader);


	_root->addChild(planet);
	_root->addChild(skyboxNode);

	projMatrix = Matrix4::Perspective(1.0f, 10000.f, (float)width / (float)height, 45.f);


	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	init = true;
}

Renderer::~Renderer() {
	delete _root;
	delete _quad;
	delete _camera;
	delete _cube;
	delete _shader;
	glDeleteTextures(1, &_texture);
}

void Renderer::UpdateScene(float dt) {
	_camera->updateCamera(dt);
	viewMatrix = _camera->buildViewMatrix();
	_frameFrustum.fromMatrix(projMatrix * viewMatrix);

	_root->update(dt);
}

void Renderer::buildNodeLists(SceneNode* from) {
	if (!from->getIsFrustrumCheckable() || _frameFrustum.isInsideFrustum(*from)) {
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

void Renderer::RenderScene() {
	buildNodeLists(_root);
	sortNodeLists();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	drawNodes();
	clearNodeLists();
}

void Renderer::clearNodeLists() {
	_transparentNodeList.clear();
	_nodeList.clear();
}

void Renderer::drawSkybox() {
	glDepthMask(GL_FALSE);

	BindShader(_skyboxShader);

	UpdateShaderMatrices();

	_quad->Draw();
	glDepthMask(GL_TRUE);
}