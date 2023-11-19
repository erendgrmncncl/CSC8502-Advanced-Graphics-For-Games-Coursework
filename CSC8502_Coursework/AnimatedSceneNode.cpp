#include "AnimatedSceneNode.h"
#include "../nclgl/MeshAnimation.h"
#include "../nclgl/MeshMaterial.h"

AnimatedSceneNode::AnimatedSceneNode(Mesh* animatedMesh, MeshAnimation* animatedMeshAnimation, MeshMaterial* animatedMeshMaterial) : SceneNode(animatedMesh){
	_meshAnimation = animatedMeshAnimation;
	_meshMaterial = animatedMeshMaterial;
	for (int i = 0; i < _mesh->GetSubMeshCount(); ++i) {
		const MeshMaterialEntry* matEntry = _meshMaterial->GetMaterialForLayer(i);

		const string* fileName = nullptr;
		matEntry->GetEntry("Diffuse", &fileName);
		string path = TEXTUREDIR + *fileName;
		GLuint texID = SOIL_load_OGL_texture(path.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS | SOIL_FLAG_INVERT_Y);
		_materialTextures.emplace_back(texID);
	}

	_currentFrame = 0;
	_currentFrameTime = 0.f;
}

void AnimatedSceneNode::setUpShader(OGLRenderer& renderer){
	
	tempModelMatrix = renderer.getModelMatrix();
	renderer.BindShader(_shader);
	Matrix4 modelMatrix = _transform * Matrix4::Scale(getModelScale());

	glUniform1i(glGetUniformLocation(_shader->GetProgram(), "diffuseTex"), 0);
	renderer.setModelMatrix(modelMatrix);
	renderer.UpdateShaderMatrices();

	vector<Matrix4> frameMatrices;
	const Matrix4* invBindPose = _mesh->GetInverseBindPose();
	const Matrix4* frameData = _meshAnimation->GetJointData(_currentFrame);
	int jointCount = _mesh->GetJointCount();
	for (unsigned int i = 0; i < jointCount; i++) {
		frameMatrices.emplace_back(frameData[i] * invBindPose[i]);
	}

	int j = glGetUniformLocation(_shader->GetProgram(), "joints");
	glUniformMatrix4fv(j, frameMatrices.size(), false, (float*)frameMatrices.data());
	renderer.setModelMatrix(tempModelMatrix);
}

void AnimatedSceneNode::draw(OGLRenderer& renderer, bool isDrawingForShadow){
	if (isDrawingForShadow){
		//TODO: draw shadow anim. Set up shadow anim shader in renderer.
		return;
	}

	if (_shader){
		setUpShader(renderer);
	}

	for (int i = 0; i < _mesh->GetSubMeshCount(); i++) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, _materialTextures[i]);
		_mesh->DrawSubMesh(i);
	}
	renderer.setModelMatrix(tempModelMatrix);
}

void AnimatedSceneNode::update(float dt){
	SceneNode::update(dt);
	_currentFrameTime -= dt;
	while (_currentFrameTime < .0f) {
		_currentFrame = (_currentFrame + 1) % _meshAnimation->GetFrameCount();
		_currentFrameTime += 1.f / _meshAnimation->GetFrameRate();
	}
}
