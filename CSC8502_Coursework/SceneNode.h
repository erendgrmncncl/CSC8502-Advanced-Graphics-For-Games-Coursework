#pragma once

#pragma once

#include "../nclgl/Matrix4.h"
#include "../nclgl/Vector3.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/Mesh.h"
#include <vector>

class OGLRenderer;

class SceneNode {
public:
	SceneNode(Mesh* mesh = nullptr, Vector4 colour = Vector4(1, 1, 1, 1));
	~SceneNode();

	void setTransform(const Matrix4& matrix);
	const Matrix4& getTransform() const;
	Matrix4 getWorldTransform() const;

	Vector4 getColour() const;
	void setColour(Vector4 colour);

	Vector3 getModelScale() const;
	void setModelScale(Vector3 modelScale);
	void setScale(float scale);

	Mesh* getMesh() const;
	void setMesh(Mesh* mesh);

	float getBoundingRadius() const;
	void setBoundingRadius(float boundingRadius);

	float getCameraDistance() const;
	void setCameraDistance(float distanceFromCamera);

	Shader* getShader() const;
	void setShader(Shader* shader);

	std::string& getNodeName();
	void setNodeName(const std::string& nodeName);

	bool getIsFrustrumCheckable() const;
	void setIsFrustrumCheckable(bool isFrustrumCheckable);

	GLuint getTexture() const;
	void setTexture(GLuint texture);
	static bool compareByCameraDistance(SceneNode* firstNode, SceneNode* secondNode);

	void addChild(SceneNode* child);
	void removeChild(SceneNode* child);

	virtual void update(float dt);
	virtual void draw(OGLRenderer& renderer);

	std::vector<SceneNode*>::const_iterator getChildIteratorStart();
	std::vector<SceneNode*>::const_iterator getChildIteratorEnd();

protected:
	bool _isFrustrumCheckable = true;

	float _boundingRadius;
	float _distanceFromCamera;

	GLuint _texture;
	SceneNode* _parent;
	Mesh* _mesh;
	Shader* _shader = nullptr;
	Matrix4 _worldTransform;
	Matrix4 _transform;
	Vector3 _modelScale;
	Vector4 _colour;

	std::string _nodeName;

	std::vector<SceneNode*> _children;

	virtual void setUpShader(OGLRenderer& renderer);
	virtual void postDraw();
};