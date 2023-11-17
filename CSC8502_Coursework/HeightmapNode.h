#pragma once
#include "SceneNode.h"

class HeightMap;
class Light;
class Camera;

struct HeightMapNodeParameter {
	std::string _heightMapNoisePath;
	GLuint _heightMapTexture;
	GLuint _bumpTexture;
	GLuint _waterTexture;
	GLuint _currentCubeMap;
	GLuint _shadowTexture;

	Shader* _waterShader;
	Mesh* _waterMesh;

	HeightMapNodeParameter(const std::string& heightMapNoisePath, 
		GLuint heightmapTexture, GLuint bumpTexture, 
		GLuint waterTexture, GLuint currentCubeMap, GLuint shadowTexture,
		Mesh* waterMesh, Shader* waterShader);


};

class HeightMapNode : public SceneNode {
public:
	HeightMapNode(Shader* _shader, Camera* camera, const HeightMapNodeParameter& parameters);
	Vector3 getHeightMapSize() const;

	void setLight(Light* light);
	Light* getLight();

	float getWaterCycle() const;
	void setWaterCycle(float waterCycle);

	float getWaterRotate() const;
	void setWaterRotate(float waterCycle);

protected:
	float _waterCycle = 0.0f;
	float _waterRotate = 0.0f;

	GLuint _texture;
	GLuint _waterTexture;
	GLuint _currentCubeMap;

	HeightMap* _heightmap;
	Shader* _waterShader;

	Mesh* _waterMesh;

	void setUpShader(OGLRenderer& renderer) override;
	void draw(OGLRenderer& renderer, bool isDrawingForShadows = false) override;
	void drawWater(OGLRenderer& renderer);
	void update(float dt) override;
};