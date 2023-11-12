#include "Light.h"

Light::Light()
{
}

Light::Light(const Vector3& position, const Vector4& colour, float radius) {
	_position = position;
	_colour = colour;
	_radius = radius;
}

Light::Light(const Vector3& position, const Vector4& colour, float radius, const Vector4& specularityColour) {
	_position = position;
	_colour = colour;
	_radius = radius;
	_specularityColour = specularityColour;
}

Light::~Light()
{
}

Vector3 Light::getPosition() const {
	return _position;
}

float Light::getRadius() const {
	return _radius;
}

Vector4 Light::getColour() const {
	return _colour;
}

void Light::setColour(const Vector4& val) {
	_colour = val;
}

Vector4 Light::getSpeculariyColour() const{
	return _specularityColour;
}

void Light::setSpacularityColour(Vector4& val){
	_specularityColour = val;
}

void Light::setRadius(float val) {
	_radius = val;
}

void Light::setPosition(const Vector3& val) {
	_position = val;
}