#pragma once

#include "../nclgl/OGLRenderer.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& window);
	~Renderer();
	void RenderScene() override;
protected:
	Mesh* triangle;
	Shader* basicShader;
};