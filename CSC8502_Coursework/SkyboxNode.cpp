#include "SkyboxNode.h"
#include "../nclgl/OGLRenderer.h"
void SkyboxNode::setUpShader(OGLRenderer& renderer)
{
	glDepthMask(GL_FALSE);
	renderer.BindShader(_shader);

	renderer.UpdateShaderMatrices();
}

void SkyboxNode::postDraw(){
	glDepthMask(GL_TRUE);
}
