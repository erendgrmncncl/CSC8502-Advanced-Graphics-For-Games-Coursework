#include "Plane.h"

Plane::Plane(){

}

Plane::Plane(const Vector3& normal, float distance, bool normalise){
	if (normalise) {
		float length = sqrt(Vector3::Dot(normal, normal));

		_normal = normal / length;
		_distance = distance / length;
	}
	else	{
		_normal = normal;
		_distance = distance;
	}
}

Plane::~Plane(){

}

void Plane::setNormal(const Vector3& normal){
	_normal = normal;
}

Vector3 Plane::getNormal() const{
	return _normal;
}

void Plane::setDistance(float distance){
	_distance = distance;
}

float Plane::getDistance() const{
	return _distance;
}

bool Plane::sphereInPlane(const Vector3& position, float radius) const{
	if (Vector3::Dot(position, _normal) + _distance <= -radius){
		return false;
	}
	return true;
}
