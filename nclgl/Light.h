#pragma once

#include "Vector4.h"
#include "Vector3.h"

class Light {
public:
	Light();
	Light(const Vector3& position, const Vector4& colour, float radius);
	Light(const Vector3& position, const Vector4& colour, float radius, const Vector4& specularityColour);
	~Light();

	Vector3 getPosition() const;
	void setPosition(const Vector3& val);

	float getRadius() const;
	void setRadius(float val);

	Vector4 getColour() const;
	void setColour(const Vector4& val);

	Vector4 getSpeculariyColour() const;
	void setSpacularityColour(Vector4& val);

protected:
	Vector3 _position;
	float _radius;
	Vector4 _colour;
	Vector4 _specularityColour;
};