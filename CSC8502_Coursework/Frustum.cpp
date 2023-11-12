#include "Frustum.h"
#include "../nclgl/Matrix4.h"
#include "SceneNode.h"

namespace {
	constexpr const int PLANE_COUNT = 6;
}

Frustum::Frustum(){
}

Frustum::~Frustum(){

}

void Frustum::fromMatrix(const Matrix4& mat){
	Vector3 xAxis = Vector3(mat.values[0], mat.values[4], mat.values[8]);
	Vector3 yAxis = Vector3(mat.values[1], mat.values[5], mat.values[9]);
	Vector3 zAxis = Vector3(mat.values[2], mat.values[6], mat.values[10]);
	Vector3 wAxis = Vector3(mat.values[3], mat.values[7], mat.values[11]);

	//RIGHT
	planes[0] = Plane(wAxis - xAxis, mat.values[15] - mat.values[12], true);

	//LEFT
	planes[1] = Plane(wAxis + xAxis, mat.values[15] + mat.values[12], true);

	//BOTTOM
	planes[2] = Plane(wAxis + yAxis, mat.values[15] + mat.values[13], true);

	//TOP
	planes[3] = Plane(wAxis - yAxis, mat.values[15] - mat.values[13], true);

	//NEAR
	planes[4] = Plane(wAxis + zAxis, mat.values[15] + mat.values[14], true);

	//FAR
	planes[5] = Plane(wAxis - zAxis, mat.values[15] - mat.values[14], true);


}

bool Frustum::isInsideFrustum(SceneNode& node){
	for (int i = 0; i < 6; i++) {
		if (!planes[i].sphereInPlane(node.getWorldTransform().GetPositionVector(), node.getBoundingRadius())) {
			return false;
		}
	}

	return true;
}
