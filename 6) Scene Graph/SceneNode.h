#pragma once

#include "../nclgl/Matrix4.h"
#include "../nclgl/Vector3.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/Mesh.h"
#include <vector>

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

	void addChild(SceneNode* child);

	virtual void update(float dt);
	virtual void draw(const OGLRenderer& renderer);
	
	std::vector<SceneNode*>::const_iterator getChildIteratorStart();
	std::vector<SceneNode*>::const_iterator getChildIteratorEnd();

protected:
	SceneNode* _parent;
	Mesh* _mesh;
	Matrix4 _worldTransform;
	Matrix4 _transform;
	Vector3 _modelScale;
	Vector4 _colour;

	std::vector<SceneNode*> _children;
};