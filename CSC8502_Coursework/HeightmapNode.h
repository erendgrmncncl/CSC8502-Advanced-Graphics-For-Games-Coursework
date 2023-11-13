#pragma once
#include "SceneNode.h"

class HeightMap;

struct HeightMapNodeParameter {
	std::string _heightMapNoisePath;
	std::string _heightMapTexPath;
	std::string _heightMapBumpTexPath;

	HeightMapNodeParameter(const std::string& heightMapNoisePath, const std::string& heightMapTexPath, const std::string& heightMapBumpTexPath);

};

class HeightMapNode : public SceneNode {
public:
	HeightMapNode(Shader* _shader, const HeightMapNodeParameter& parameters);
	Vector3 getHeightMapSize() const;
protected:
	GLuint _bumpTexture;
	GLuint _texture;
	HeightMap* _heightmap;


	void setUpShader(OGLRenderer& renderer) override;
	void draw(OGLRenderer& renderer) override;
};