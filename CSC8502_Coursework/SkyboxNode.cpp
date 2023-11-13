#include "SkyboxNode.h"
#include "../nclgl/OGLRenderer.h"
void SkyboxNode::setUpShader(OGLRenderer& renderer)
{
	glDepthMask(GL_FALSE);
	renderer.BindShader(_shader);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _texture);

	renderer.UpdateShaderMatrices();
}

void SkyboxNode::postDraw(){
	glDepthMask(GL_TRUE);
}
