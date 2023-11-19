#include "HeightmapNode.h"
#include "../nclgl/HeightMap.h"
#include "../nclgl/Light.h"
#include "../nclgl/Camera.h"

HeightMapNode::HeightMapNode(Shader* _shader, Camera* camera, const HeightMapNodeParameter& parameters) : SceneNode() {
	setShader(_shader);
	_heightmap = new HeightMap(parameters._heightMapNoisePath);
	_texture = parameters._heightMapTexture;
	_bumpTexture = parameters._bumpTexture;
	_mesh = _heightmap;
	_camera = camera;
	_waterTexture = parameters._waterTexture;
	_waterShader = parameters._waterShader;
	_currentCubeMap = parameters._currentCubeMap;
	_waterMesh = parameters._waterMesh;
	_shadowTexture = parameters._shadowTexture;
}

void HeightMapNode::setUpShader(OGLRenderer& renderer) {
	renderer.BindShader(_shader);
	renderer.SetShaderLight(*_light);
	
	glUniform1i(glGetUniformLocation(_shader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(_shader->GetProgram(), "bumpTex"), 1);
	glUniform1i(glGetUniformLocation(_shader->GetProgram(),"shadowTex"), 2);
	glUniform3fv(glGetUniformLocation(_shader->GetProgram(), "cameraPos"), 1, (float*)&_camera->getPosition());

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _bumpTexture);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, _shadowTexture);

	Matrix4 modelMatrixToSet = renderer.getModelMatrix();
	modelMatrixToSet.ToIdentity();
	renderer.setModelMatrix(modelMatrixToSet);
	renderer.UpdateShaderMatrices();
}

Vector3 HeightMapNode::getHeightMapSize() const
{
	return _heightmap->getHeightmapSize();
}

void HeightMapNode::setLight(Light* light) {
	_light = light;
}

Light* HeightMapNode::getLight()
{
	return _light;
}

void HeightMapNode::draw(OGLRenderer& renderer, bool isDrawingForShadows) {
	if (isDrawingForShadows)
	{
		_mesh->Draw();
		return;
	}
	if (_shader)
		setUpShader(renderer);
	_mesh->Draw();
	drawWater(renderer);
	postDraw(renderer);
}

void HeightMapNode::drawWater(OGLRenderer& renderer) {
	renderer.BindShader(_waterShader);

	glUniform3fv(glGetUniformLocation(_waterShader->GetProgram(), "cameraPos"), 1, (float*)&_camera->getPosition());

	glUniform1i(glGetUniformLocation(_waterShader->GetProgram(), "diffuseTex"), 0);
	glUniform1i(glGetUniformLocation(_waterShader->GetProgram(), "cubeTex"), 2);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _waterTexture);


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _currentCubeMap);

	Vector3 hSize = _heightmap->getHeightmapSize();

	renderer.setModelMatrix(Matrix4::Translation(hSize * 0.5f - Vector3(0, 35.0f, 0)) * Matrix4::Scale(hSize * 1.f) * Matrix4::Rotation(90, Vector3(1, 0, 0)));


	renderer.setTextureMatrix(Matrix4::Translation(Vector3(_waterCycle, .0f, _waterCycle)) * Matrix4::Scale(Vector3(10, 10, 10)) * Matrix4::Rotation(_waterRotate, Vector3(0, 0, 1)));

	renderer.UpdateShaderMatrices();
	_waterMesh->Draw();
	Matrix4 textureMatrix = renderer.getModelMatrix();
	textureMatrix.ToIdentity();
	renderer.setTextureMatrix(textureMatrix);
}

void HeightMapNode::update(float dt){
	_waterRotate += dt * 2.f; //2degree a second.
	_waterCycle += dt * .25f; //10 units a second.
}

void HeightMapNode::postDraw(OGLRenderer& renderer){
}

HeightMapNodeParameter::HeightMapNodeParameter(const std::string& heightMapNoisePath, GLuint heighMapTexture, GLuint bumpTexture, GLuint waterTexture, GLuint currentCubeMap, GLuint shadowTexture,  Mesh* waterMesh, Shader* waterShader)
{
	_heightMapTexture = heighMapTexture;
	_heightMapNoisePath = heightMapNoisePath;
	_bumpTexture = bumpTexture;
	_waterShader = waterShader;
	_waterTexture = waterTexture;
	_currentCubeMap = currentCubeMap;
	_waterMesh = waterMesh;
	_shadowTexture = shadowTexture;
}

void HeightMapNode::setWaterCycle(float waterCycle){
	_waterCycle = waterCycle;
}

float HeightMapNode::getWaterRotate() const{
	return _waterRotate;
}

void HeightMapNode::setWaterRotate(float waterCycle){
	_waterRotate = waterCycle;
}

float HeightMapNode::getWaterCycle() const{
	return _waterCycle;
}
