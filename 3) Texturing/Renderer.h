#pragma once
#include "../nclgl/OGLRenderer.h"

class Camera;

class Renderer : public OGLRenderer {
public:
	Renderer(Window& parent);
	virtual ~Renderer();

	virtual void RenderScene();
	virtual void UpdateScene(float dt);
	void SwitchToPerspective();
	void SwitchToOrthographic();
	void UpdateTextureMatrix(float rotation);
	void ToggleRepeating();
	void ToggleFiltering();

	inline void SetScale(float s) { _scale = s; }
	inline void SetRotation(float r) { _rotation = r; }
	inline void SetPosition(Vector3 p) { position = p; }
	inline void SetFOV(float fov) { _fov = fov; }
	inline void SetBlendRatio(float blendRatio) { _blendRatio = blendRatio; }

	inline float GetBlendRatio() { return _blendRatio; }
	inline float GetFOV() { return _fov; }

protected:
	bool _isRepeating;
	bool _isFiltering;

	float _scale;
	float _rotation;
	float _fov;
	float _blendRatio;

	Vector3 position;

	Mesh* _triangle;
	Shader* _shader;
	Camera* _camera;
	GLuint _texture;
	GLuint _second_texture;
};