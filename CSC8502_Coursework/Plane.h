#pragma once

#include "../nclgl/vector3.h"

class Plane {
public:
	Plane();
	Plane(const Vector3& normal, float distance, bool normalise = false);
	~Plane();

	void setNormal(const Vector3& normal);
	Vector3 getNormal() const;

	void setDistance(float distance);
	float getDistance() const;

	bool sphereInPlane(const Vector3& position, float radius) const;
protected:
	Vector3 _normal;
	float _distance;
};