#include "SpaceObject.h"

SpaceObjectParams::SpaceObjectParams(Vector3 transform, Vector3 scale, const char* texturePath) {
	_transform = transform;
	_scale = scale;
	_texturePath = texturePath;
}

SpaceObject::SpaceObject(Mesh* mesh, GLuint texture, Shader* shader, SpaceObjectParams params) : SceneNode(mesh) {
	_texture = texture;
	_shader = shader;
	setModelScale(params._scale);
	setTransform(Matrix4::Translation(params._transform));
	setBoundingRadius(20.f);
}