#include <vector>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include <glm\glm.hpp>
using namespace glm;

bool loadOBJ(const char * path, std::vector <glm::vec3> out_v, std::vector <glm::vec2> out_vt, std::vector <glm::vec3> out_vn) {

std::vector <unsigned int> vIndices, vtIndices, vnIndices;
std::vector <glm::vec3> temp_v; // Coordonnées d'un sommet
std::vector <glm::vec2> temp_vt; // Coordonnées d'une texture
std::vector <glm::vec3> temp_vn; // Coordonnées d'une normale

FILE * file = fopen(path, "r");
if (file == NULL) {
	printf("Impossible to open the file !\n");
	return false;
}

while (1) {
	char lineHeader[128];
	// read the first word of the line
	int res = fscanf(file, "%s", lineHeader);
	if (res == EOF)
		break;
	if (strcmp(lineHeader, "v") == 0) {
		glm::vec3 vertex;
		fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
		temp_v.push_back(vertex);
	}
	else if (strcmp(lineHeader, "vt") == 0) {
		glm::vec2 vt;
		fscanf(file, "%f %f\n", &vt.x, &vt.y);
		temp_vt.push_back(vt);
	}
	else if (strcmp(lineHeader, "vn") == 0) {
		glm::vec3 vn;
		fscanf(file, "%f %f %f\n", &vn.x, &vn.y, &vn.z);
		temp_vn.push_back(vn);
	}
	else if (strcmp(lineHeader, "f") == 0) {
		std::string vertex1, vertex2, vertex3;
		unsigned int vIndex[3], vtIndex[3], vnIndex[3];
		int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vIndex[0], &vtIndex[0], &vnIndex[0], &vIndex[1], &vtIndex[1], &vnIndex[1], &vIndex[2], &vtIndex[2], &vnIndex[2]);
		if (matches != 9) {
			printf("File can't be read by our simple parser : ( Try exporting with other options\n");
			return false;
		}
		vIndices.push_back(vIndex[0]);
		vIndices.push_back(vIndex[1]);
		vIndices.push_back(vIndex[2]);
		vtIndices.push_back(vtIndex[0]);
		vtIndices.push_back(vtIndex[1]);
		vtIndices.push_back(vtIndex[2]);
		vnIndices.push_back(vnIndex[0]);
		vnIndices.push_back(vnIndex[1]);
		vnIndices.push_back(vnIndex[2]);
	}

	for (unsigned int i = 0; i < vIndices.size(); i++) {
		unsigned int vIndex = vIndices[i];
		glm::vec3 vertex = temp_v[vIndex - 1];
		out_v.push_back(vertex);
	}

}

int main() {
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > uvs;
	std::vector< glm::vec3 > normals; 
	bool res = loadOBJ("cube.obj", vertices, uvs, normals);
}