#include "SpaceObject.h"

SpaceObject::SpaceObject(Mesh* mesh, GLuint texture, Shader* shader): SceneNode(mesh){
	_texture = texture;
	setModelScale(Vector3(150, 150, 150));
	setTransform(Matrix4::Translation(Vector3(0, 35, 0)));
	setBoundingRadius(150.f);
	if (shader != nullptr);
		setShader(shader);
}
