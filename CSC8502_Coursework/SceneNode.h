#pragma once

#pragma once

#include "../nclgl/Matrix4.h"
#include "../nclgl/Vector3.h"
#include "../nclgl/Vector4.h"
#include "../nclgl/Mesh.h"
#include <vector>

class OGLRenderer;
class Camera;
class MeshMaterial;

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

	bool getIsAnimated() const;
	void setIsAnimated(bool isFrustrumCheckable);

	GLuint getTexture() const;
	void setTexture(GLuint texture);

	MeshMaterial* getMeshMaterial();
	void setMeshMaterial(MeshMaterial* meshMaterial);

	GLuint getShadowTexture() const;
	void setShadowTexture(GLuint shadowTexture);

	GLuint getBumpTexture() const;
	void setBumpTexture(GLuint bumpTexture);

	static bool compareByCameraDistance(SceneNode* firstNode, SceneNode* secondNode);

	void addChild(SceneNode* child);
	void removeChild(SceneNode* child);

	Light* getLight();
	void setLight(Light* light);

	Camera* getCamera();
	void setCamera(Camera* camera);
	
	virtual void update(float dt);
	virtual void draw(OGLRenderer& renderer, bool isDrawingForShadows = false);
	void initMovement(std::vector<Vector3> nodesToVisit, bool isLoopable, float speed = 10.f);

	std::vector<SceneNode*>::const_iterator getChildIteratorStart();
	std::vector<SceneNode*>::const_iterator getChildIteratorEnd();

	virtual void setUpShader(OGLRenderer& renderer);

protected:
	bool _isAnimatedNode = false;
	
	bool _isMoveable = false;
	bool _isLoopingBetweenNodes = false;

	int _currentNodeIndex = 0;

	float _boundingRadius;
	float _distanceFromCamera;
	float _speed = 0.f;

	GLuint _texture;
	GLuint _shadowTexture;
	GLuint _bumpTexture = 0;

	MeshMaterial* _meshMaterial;

	SceneNode* _parent = nullptr;
	Mesh* _mesh = nullptr;
	Shader* _shader = nullptr;
	Light* _light = nullptr;
	Camera* _camera = nullptr;

	Matrix4 _worldTransform;
	Matrix4 _transform;
	Vector3 _modelScale;
	Vector4 _colour;

	std::string _nodeName;

	std::vector<SceneNode*> _children;
	vector<GLuint> _materialTextures;

	Vector3 _currentNodeToVisit;
	std::vector<Vector3> _nodesToVisit;
	std::vector<Vector3> _completedNodes;

	virtual void postDraw(OGLRenderer& renderer);
	virtual void handleMovement(float dt);
	virtual void moveTowards(float currentSpeed, Vector3 target, Vector3 currentPosition);
};