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
class MeshAnimation;
class MeshMaterial;

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
	Mesh* _animatedMesh;
	MeshAnimation* _animateMeshAnimation;
	MeshMaterial* _animatedMeshMaterial;

	GLuint _heightMapTexture;
	GLuint _bumpTexture;
	GLuint _cubeMap;
	GLuint _planetCubemap;
	GLuint _treeTexture;
	GLuint _shadowTex;
	GLuint _waterTex;

	GLuint _shadowFBO;
	GLuint _postProcessFBO;
	GLuint _bufferFBO;

	GLuint _bufferColourTex[2];
	GLuint _bufferDepthTex;

	Shader* _shader;
	Shader* _perPixelSceneShader;
	Shader* _shadowSceneShader;
	Shader* _skyboxShader;
	Shader* _lightShader;
	Shader* _reflectShader;
	Shader* _shadowShader;
	Shader* _animationShader;
	Shader* _postProcessShader;

	Frustum _frameFrustum;

	Vector3 _heightMapSize;
	
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
	void drawPostProcess();
	void presentScene();
};