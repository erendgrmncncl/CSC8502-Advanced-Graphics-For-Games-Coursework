#include "HeightmapNode.h"
#include "../nclgl/HeightMap.h"

HeightMapNode::HeightMapNode(Shader* _shader, const HeightMapNodeParameter& parameters) : SceneNode(){
	setShader(_shader);
	_heightmap = new HeightMap(parameters._heightMapNoisePath);
	_mesh = _heightmap;
	_texture = SOIL_load_OGL_texture(parameters._heightMapTexPath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
	_bumpTexture = SOIL_load_OGL_texture(parameters._heightMapBumpTexPath.c_str(), SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_MIPMAPS);
}

void HeightMapNode::setUpShader(OGLRenderer& renderer){
	//renderer.BindShader(_lightShader);
	//TODO(eren.degirmenci): bind light after draw
	//SetShaderLight(*_light);
	//glUniform1i(glGetUniformLocation(_lightShader->GetProgram(), "diffuseTex"), 0);
	renderer.BindShader(_shader);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _texture);

	//glUniform1i(glGetUniformLocation(_lightShader->GetProgram(), "bumpTex"), 1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, _bumpTexture);

	renderer.getModelMatrix().ToIdentity();
	renderer.getTextureMatrix().ToIdentity();

	renderer.UpdateShaderMatrices();
}

Vector3 HeightMapNode::getHeightMapSize() const
{
	return _heightmap->getHeightmapSize();
}

void HeightMapNode::draw(OGLRenderer& renderer){
	if (_shader)
		setUpShader(renderer);
	_heightmap->Draw();
	postDraw();
}

HeightMapNodeParameter::HeightMapNodeParameter(const std::string& heightMapNoisePath, const std::string& heightMapTexPath, const std::string& heightMapBumpTexPath)
{
	_heightMapBumpTexPath = heightMapBumpTexPath;
	_heightMapNoisePath = heightMapNoisePath;
	_heightMapTexPath = heightMapTexPath;
}
