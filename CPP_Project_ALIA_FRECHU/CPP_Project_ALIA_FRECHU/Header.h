#include <math.h>
#include <glm\glm.hpp>

// Returns the length of the vec2
float Length2D(glm::vec2 tempv){
	float X = tempv.x;
	float Y = tempv.y;
	return sqrt(X * X + Y * Y);
}

// Returns the length of the vec3
float Length3D(glm::vec3 tempv) {
	float X = tempv.x;
	float Y = tempv.y;
	float Z = tempv.z;
	return sqrt(X * X + Y * Y + Z * Z);
}

// Normalizes the vec2
glm::vec2 Normalize2D(glm::vec2 tempv) {
	float length = Length2D(tempv);
	float X = tempv.x;
	float Y = tempv.y;
	if (length != 0) {
		tempv.x = X / length;
		tempv.y = Y / length;
	}

	return tempv;
}

// Normalizes the vec3
glm::vec3 Normalize3D(glm::vec3 tempv) {
	float length = Length3D(tempv);
	float X = tempv.x;
	float Y = tempv.y;
	float Z = tempv.z;
	if (length != 0) {
		tempv.x = X / length;
		tempv.y = Y / length;
		tempv.z = Z / length;
	}

	return tempv;
}
