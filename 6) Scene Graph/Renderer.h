#pragma once

#include "../nclgl/OGLRenderer.h"
#include "../nclgl/Camera.h"
#include "SceneNode.h"
#include "CubeRobot.h"

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	~Renderer(void);

	void UpdateScene(float dt) override;
	void RenderScene() override;
protected:
	SceneNode* _root;
	Camera* _camera;
	Mesh* _cube;
	Shader* _shader;

	void drawNode(SceneNode* node);
};