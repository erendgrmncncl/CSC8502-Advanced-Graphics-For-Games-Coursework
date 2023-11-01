#include "Renderer.h"

Renderer::Renderer(Window &parent) : OGLRenderer(parent) {
	_cube = Mesh::LoadFromMeshFile("OffsetCubeY.msh");
	_camera = new Camera();

	_shader = new Shader("SceneVertex.glsl", "SceneFragment.glsl");
	if (!_shader->LoadSuccess()){
		return;
	}

	projMatrix = Matrix4::Perspective(1.0f, 10000.f, (float)width / (float)height, 45.0f);
	_camera->setPosition(Vector3(0, 30, 175));

	_root = new SceneNode();
	for (int i = 0; i < 10; i++)
	{
		auto* robot = new CubeRobot(_cube);
		auto transform = robot->getTransform().GetPositionVector();
		if (i == 2) {
			robot->setScale(10);
		}

		robot->setTransform(
			Matrix4::Translation(Vector3(transform.x + (i*50), transform.y, transform.z)));

		_root->addChild(robot);
	}
	
	glEnable(GL_DEPTH_TEST);
	init = true;
}

Renderer::~Renderer() {
	delete _root;
	delete _shader;
	delete _camera;
}

void Renderer::UpdateScene(float dt) {
	_camera->updateCamera(dt);
	viewMatrix = _camera->buildViewMatrix();
	_root->update(dt);
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	BindShader(_shader);
	UpdateShaderMatrices();
	glUniform1i(glGetUniformLocation(_shader->GetProgram(), "diffuseTex"), 1);

	drawNode(_root);
}

void Renderer::drawNode(SceneNode* root) {
	if (root->getMesh()){
		Matrix4 model = root->getWorldTransform() * Matrix4::Scale(root->getModelScale());
		glUniformMatrix4fv(glGetUniformLocation(_shader->GetProgram(), "modelMatrix"), 1, false, model.values);

		glUniform4fv(glGetUniformLocation(_shader->GetProgram(), "nodeColour"), 1, (float*)&root->getColour());
		glUniform1i(glGetUniformLocation(_shader->GetProgram(), "useTexture"), 0);

		root->draw(*this);
	}

	for (vector<SceneNode*>::const_iterator i = root->getChildIteratorStart(); i != root->getChildIteratorEnd(); ++i) {
		drawNode(*i);
	}
}