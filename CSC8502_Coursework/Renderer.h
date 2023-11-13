#pragma once
#pragma once
#include "../nclgl/OGLRenderer.h"
#include "Frustum.h"

class Camera;
class SceneNode;
class Mesh;
class Shader;
class SkyboxNode;

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	~Renderer();

	void UpdateScene(float dt) override;
	void RenderScene() override;

protected:
	bool _sceneToggle = false;

	Vector3 m_planetSceneCameraPos;
	SceneNode* _globalRoot;
	SceneNode* _spaceRoot;
	SceneNode* _planetRoot;
	SkyboxNode* _skyboxNode;
	SceneNode* _currentSceneRoot;

	Camera* _camera;

	Mesh* _quad;
	Mesh* _sphere;
	GLuint _texture;
	GLuint _cubeMap;
	GLuint _planetCubemap;

	Shader* _shader;
	Shader* _skyboxShader;
	Shader* _lightShader;

	Frustum _frameFrustum;
	
	std::vector<SceneNode*> _transparentNodeList;
	std::vector<SceneNode*> _nodeList;
	
	void buildNodeLists(SceneNode* nodeToBuild);
	void sortNodeLists();
	void drawNodes();
	void clearNodeLists();
	void drawNode(SceneNode* nodeToDraw);
	void InitGlobalSceneNode();
	void InitSpaceSceneNodes();
	void InitPlanetSceneNodes();
	void InitSkyboxNode();
	void toggleScene();
	void onChangeScene();
};