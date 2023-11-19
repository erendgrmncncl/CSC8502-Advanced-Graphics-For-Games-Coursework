#pragma once

#include <iostream>
namespace Utility {
	float radiansToDegrees(float radians) {
		float pi = std::atan(1) * 4;
		return radians * (180.0f / pi);
	}

	float lerp(float a, float b, float t) {
		return a + t * (b - a);
	}
}