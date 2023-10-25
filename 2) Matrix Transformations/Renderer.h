#pragma once
#include "../nclgl/OGLRenderer.h"

class Camera;

class Renderer : public OGLRenderer {
public:
	Renderer(Window &parent);
	virtual ~Renderer();

	virtual void RenderScene();
	virtual void UpdateScene(float dt);
	void SwitchToPerspective();
	void SwitchToOrthographic();

	inline void SetScale(float s) { _scale = s; }
	inline void SetRotation(float r) { _rotation = r; }
	inline void SetPosition(Vector3 p) { position = p; }
	inline void SetFOV(float fov) { _fov = fov; }

	inline float GetFOV() { return _fov; }

protected:
	float _scale;
	float _rotation;
	float _fov;
	
	Vector3 position;

	Mesh* triangle;
	Shader* matrixShader;
	Camera* _camera;
};