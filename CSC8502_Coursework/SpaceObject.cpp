#include "SpaceObject.h"

SpaceObjectParams::SpaceObjectParams(Vector3 transform, Vector3 scale, Vector4 color, const char* texturePath) {
	_transform = transform;
	_scale = scale;
	_texturePath = texturePath;
	_color = color;
}

SpaceObject::SpaceObject(Mesh* mesh, GLuint texture, Shader* shader, SpaceObjectParams params) : SceneNode(mesh) {
	_texture = texture;
	_shader = shader;
	_colour = params._color;
	setModelScale(params._scale);
	setTransform(Matrix4::Translation(params._transform));
	setBoundingRadius(20.f);
}