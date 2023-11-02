#pragma once

#include "Plane.h"
class SceneNode;
class Matrix4;

class Frustum {
public:
	Frustum();
	~Frustum();

	void fromMatrix(const Matrix4 &mvp);
	bool isInsideFrustum(SceneNode& node);
protected:
	Plane planes[6];
};