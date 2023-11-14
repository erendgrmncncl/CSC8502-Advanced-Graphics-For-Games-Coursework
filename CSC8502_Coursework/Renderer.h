#pragma once
#pragma once
#include "../nclgl/OGLRenderer.h"
#include "Frustum.h"

class Camera;
class SceneNode;
class Mesh;
class Shader;
class SkyboxNode;
class Light;

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

	Light* _currentLight;

	Camera* _camera;

	Mesh* _quad;
	Mesh* _sphere;
	Mesh* _tree;

	GLuint _heightMapTexture;
	GLuint _bumpTexture;
	GLuint _cubeMap;
	GLuint _planetCubemap;
	GLuint _treeTexture;
	GLuint _shadowTex;
	GLuint _waterTex;

	GLuint _shadowFBO;

	Shader* _shader;
	Shader* _skyboxShader;
	Shader* _lightShader;
	Shader* _reflectShader;
	Shader* _shadowShader;

	Frustum _frameFrustum;
	
	std::vector<SceneNode*> _transparentNodeList;
	std::vector<SceneNode*> _nodeList;
	
	void buildNodeLists(SceneNode* nodeToBuild);
	void sortNodeLists();
	void drawNodes(bool isDrawingForShadows = false);
	void clearNodeLists();
	void drawNode(SceneNode* nodeToDraw, bool isDrawingForShadows = false);
	void InitGlobalSceneNode();
	void InitSpaceSceneNodes();
	void InitPlanetSceneNodes();
	void InitSkyboxNode();
	void toggleScene();
	void onChangeScene();
	void drawShadowScene();
};