#include "Renderer.h"
#include "../nclgl/Camera.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"
Renderer::Renderer(Window& window) : OGLRenderer(window) {
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	projMatrix = Matrix4::Perspective(1.f, 10000.f, (float)width / (float)height, 45.f);
	_camera = new Camera(-3, 0.f, Vector3(0, 1.4f, 4.f));

	_shader = new Shader("SkinningVertex.glsl", "texturedFragment.glsl");
	if (!_shader->LoadSuccess())
		return;
	_mesh = Mesh::LoadFromMeshFile("Role_T.msh");
	_anim = new MeshAnimation("Role_T.anm");
	_material = new MeshMaterial("Role_T.mat");

	for (int i = 0; i < _mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = _material->GetMaterialForLayer(i);

		const string* fileName = nullptr;
		matEntry->GetEntry("Diffuse", &fileName);
		string path = TEXTUREDIR + *fileName;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		_matTextures.emplace_back(texID);
	}

	_currentFrame = 0;
	_frameTime = 0.f;
	init = true;
}

Renderer::~Renderer() {
	delete _camera;
	delete _mesh;
	delete _anim;
	delete _material;
	delete _shader;
}

void Renderer::UpdateScene(float dt) {
	_camera->updateCamera(dt);
	viewMatrix = _camera->buildViewMatrix();
	_frameTime -= dt;
	while (_frameTime < .0f) {
		_currentFrame = (_currentFrame + 1) % _anim->GetFrameCount();
		_frameTime += 1.f / _anim->GetFrameRate();
	}
}

void Renderer::RenderScene() {
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	BindShader(_shader);

	glUniform1i(glGetUniformLocation(_shader->GetProgram(), "diffuseTex"), 0);
	UpdateShaderMatrices();

	vector<Matrix4> frameMatrices;
	const Matrix4* invBindPose = _mesh->GetInverseBindPose();
	const Matrix4* frameData = _anim->GetJointData(_currentFrame);

	for (unsigned int i = 0; i < _mesh->GetJointCount(); i++) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}

	int j = glGetUniformLocation(_shader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());

	for (int i = 0; i < _mesh->GetSubMeshCount(); i++){
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _matTextures[i]);
		_mesh->DrawSubMesh(i);
	}
}