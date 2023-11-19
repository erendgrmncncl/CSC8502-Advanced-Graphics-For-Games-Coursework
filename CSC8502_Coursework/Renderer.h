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
class HeightMap;

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	~Renderer();

	void UpdateScene(float dt) override;
	void RenderScene() override;

protected:
	bool _sceneToggle = false;
	bool _isBlurOn = false;

	float _waterCycle = 0.0f;
	float _waterRotate = 0.0f;

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
	Mesh* _lanternPlant;
	MeshAnimation* _animateMeshAnimation;
	MeshMaterial* _animatedMeshMaterial;
	MeshMaterial* _lanternPlantMeshMaterial;

	GLuint _heightMapTexture;
	GLuint _bumpTexture;
	GLuint _cubeMap;
	GLuint _planetCubemap;
	GLuint _treeTexture;
	GLuint _shadowTex;
	GLuint _waterTex;
	GLuint _lowPolyTex;
	GLuint _lowPolyBumpTex;
	GLuint _treeBumpTex;
	GLuint _rainTexture;

	GLuint _shadowFBO;
	GLuint _postProcessFBO;
	GLuint _bufferFBO;

	GLuint _bufferColourTex[2];
	GLuint _bufferDepthTex;

	Shader* _shader;
	Shader* _presentShader;
	Shader* _perPixelSceneShader;
	Shader* _shadowSceneShader;
	Shader* _skyboxShader;
	Shader* _lightShader;
	Shader* _reflectShader;
	Shader* _shadowShader;
	Shader* _animationShader;
	Shader* _postProcessShader;

	Frustum _frameFrustum;

	HeightMap* _heightMap;


	std::vector<Vector3> _planetSceneCameraNodesToVisit;
	std::vector<Vector3> _spaceSceneCameraNodesToVisit;
	
	std::vector<SceneNode*> _transparentNodeList;
	std::vector<SceneNode*> _animatedNodeList;
	std::vector<SceneNode*> _nodeList;
	
	void initCameraFollowNodes();
	void initMeshes();
	void initTextures();
	void initBuffers();
	void initShaders();


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
	void drawHeightMap();
	void drawSkybox();
	void drawWater();
};