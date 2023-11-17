#include "SkyboxNode.h"
#include "../nclgl/OGLRenderer.h"
void SkyboxNode::setUpShader(OGLRenderer& renderer)
{
	glDepthMask(GL_FALSE);
	renderer.BindShader(_shader);
	glUniform1i(glGetUniformLocation(_shader->GetProgram(), "cubeTex"), 2);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);

	renderer.UpdateShaderMatrices();
}

void SkyboxNode::postDraw(){
	glDepthMask(GL_TRUE);
}

void SkyboxNode::draw(OGLRenderer& renderer, bool isDrawingForShadows)
{
	if (isDrawingForShadows) {
		return;
	}

	if (_shader) {
		setUpShader(renderer);
	}
	if (_mesh)
		_mesh->Draw();

	postDraw();
}
