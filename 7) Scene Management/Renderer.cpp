#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "CubeRobot.h"
#include <algorithm>

Renderer::Renderer(Window& parent) : OGLRenderer(parent) {
	_camera = new Camera(.0f, .0f, Vector3(0, 100, 750.f));
	_quad = Mesh::GenerateQuad();
	_cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");

	_shader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");

	_texture = SOIL_load_OGL_texture(TEXTUREDIR"stainedglass.tga",
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID, 0);

	if (!_shader->LoadSuccess() || !_texture) {
		return;
	}

	_root = new SceneNode();
	for (int i = 0; i < 5; i++) {
		SceneNode* node = new SceneNode();
		node->setColour(Vector4(1.f, 1.f, 1.f, .5f));
		node->setTransform(Matrix4::Translation(Vector3(.0f, 100.f, -300.f + 100.f + 100 * i)));
		node->setModelScale(Vector3(100.f, 100.f, 100.f));
		node->setBoundingRadius(100.f);
		node->setMesh(_quad);
		node->setTexture(_texture);
		_root->addChild(node);
	}

	_root->addChild(new CubeRobot(_cube));

	projMatrix = Matrix4::Perspective(1.0f, 10000.f, (float)width / (float)height, 45.f);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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
	if (_frameFrustum.isInsideFrustum(*from)) {
		Vector3 dir = from->getWorldTransform().GetPositionVector() - _camera->getPosition();
		from->setCameraDistance(Vector3::Dot(dir, dir));
		if (from->getColour().w < 1.f) {
			_transparentNodeList.push_back(from);
		}
		else {
			_nodeList.push_back(from);
		}
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
		Matrix4 model = node->getWorldTransform() * Matrix4::Scale(node->getModelScale());
		glUniformMatrix4fv(glGetUniformLocation(_shader->GetProgram(), "modelMatrix"), 1, false, model.values);
		glUniform4fv(glGetUniformLocation(_shader->GetProgram(), "nodeColour"), 1, (float*)&node->getColour());
		
		_texture = node->getTexture();
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _texture);

		glUniform1i(glGetUniformLocation(_shader->GetProgram(), "useTexture"), _texture);

		node->draw(*this);
	}
}

void Renderer::RenderScene() {
	buildNodeLists(_root);
	sortNodeLists();
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(_shader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(_shader->GetProgram(), "diffuseTex"), 0);
	drawNodes();
	clearNodeLists();
}

void Renderer::clearNodeLists() {
	_transparentNodeList.clear();
	_nodeList.clear();
}