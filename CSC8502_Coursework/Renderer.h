#pragma once
#pragma once
#include "../nclgl/OGLRenderer.h"
#include "Frustum.h"

class Camera;
class SceneNode;
class Mesh;
class Shader;

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	~Renderer();

	void UpdateScene(float dt) override;
	void RenderScene() override;

protected:
	void buildNodeLists(SceneNode* nodeToBuild);
	void sortNodeLists();
	void drawNodes();
	void clearNodeLists();
	void drawNode(SceneNode* nodeToDraw);
	void drawSkybox();

	SceneNode* _root;
	Camera* _camera;

	Mesh* _quad;
	Mesh* _cube;
	Mesh* _sphere;
	GLuint _texture;
	GLuint _skyboxCubeMap;

	Shader* _shader;
	Shader* _skyboxShader;
	Shader* _lightShader;

	Frustum _frameFrustum;
	
	std::vector<SceneNode*> _transparentNodeList;
	std::vector<SceneNode*> _nodeList;
};