#pragma once

#include "SceneNode.h"
#include "../nclgl/OGLRenderer.h"

class SkyboxNode : public SceneNode {
protected:
	void setUpShader(OGLRenderer& renderer) override;
	void postDraw(OGLRenderer& renderer) override;
	void draw(OGLRenderer& renderer, bool isDrawingForShadows) override;
};