#include <vector>
#include <cstdio>
#include <stdlib.h>
#include <cstring>
#include <glm\glm.hpp>
#include <istream>
#include <fstream>
#include <Header.h>

using namespace glm;

// An OBJ file is composed of a list of vertices (v), of textures (vt), normals (vn) and faces (f).
// Vertices defined as a x, y, z : v	0.1		3.13	-7.9
// Texture coordinates defined as a s, t in the range[0:1] : vt		0.2		0.76
// Normals defined as a x, y, z : vn	0.2		-0.3	0.6
// Face defined as a references of vertices, textures, and normals. 
// Examples of faces : f v1			v2			v3
//  				   f v1/vt1		v2/vt2		v3/vt3
//					   f v1/vt1/vn1 v2/vt2/vn2	v3/vt3/vn3 
//					   f v1//vn1	v2//vn2		v3//vn3
// In the face definition elements are indexed and indexes start at 1 but can also be negative (-1 is the last vertice).
//

// We will need the OpenGL library GLM.

bool loadOBJ(const char * path, std::vector <glm::vec3> out_v, std::vector <glm::vec2> out_vt, std::vector <glm::vec3> out_vn) {

	// We are going to use vec2 and vec3 to store the coordinates of these elements. They are vector classes that can store 2/3 coordinates

	std::vector <unsigned int> vIndices, vtIndices, vnIndices;
	std::vector <glm::vec3> temp_v; // Vertice coordinates
	std::vector <glm::vec2> temp_vt; // Texture coordinates
	std::vector <glm::vec3> temp_vn; // Normals coordinates

	// We need to open the OBJ file using fopen ("rb" parameter is for non txt files)

	FILE * file = fopen(path, "rb");

	// If file is empty then error message
	if (file == NULL) {
		printf("Impossible to open the file !");
		return false;
	}

	// We need the size of the OBJ file to go to all the lins
	fseek(file, 0, SEEK_END);
	const int size = ftell(file);
	fseek(file, 0, SEEK_SET);

	while (1) {
		char lineHeader[size];
		// Read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break;
		// We need to check whether the element is v, vt, vn or f using string comparator strcmp
		// Then we store all the vertices, textures and normals in vec2 and vec3
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
		// For the faces, we will assume it will be the following format : v/vt/vn 
		// Exctracts all the indices for each face
		else if (strcmp(lineHeader, "f") == 0) {
			std::string vertex1, vertex2, vertex3;
			unsigned int vIndex[3], vtIndex[3], vnIndex[3];
			int matches = fscanf(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vIndex[0], &vtIndex[0], &vnIndex[0], &vIndex[1], &vtIndex[1], &vnIndex[1], &vIndex[2], &vtIndex[2], &vnIndex[2]);
			if (matches != 9) {
				printf("File can't be read.\n");
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
		// Select the corresponding vertice, texture, normal and store it in a vector such that for indix 1/2/3 we have the first face etc...
		for (unsigned int i = 0; i < vIndices.size(); i++) {
			unsigned int vIndex = vIndices[i];
			glm::vec3 vertex = temp_v[vIndex - 1];
			out_v.push_back(vertex);
		}
		for (unsigned int i = 0; i < vtIndices.size(); i++) {
			unsigned int vtIndex = vtIndices[i];
			glm::vec2 vt = temp_vt[vtIndex - 1];
			out_vt.push_back(vt);
		}
		for (unsigned int i = 0; i < vnIndices.size(); i++) {
			unsigned int vnIndex = vnIndices[i];
			glm::vec3 vn = temp_vn[vnIndex - 1];
			out_vn.push_back(vn);
		}
	}
}

int main(){
	std::vector< glm::vec3 > vertices;
	std::vector< glm::vec2 > vt;
	std::vector< glm::vec3 > vn; 
	bool res = loadOBJ("cube.obj", vertices, vt, vn);

	for (int i = 0; i < vertices.size(), i++) {
		Normalize2D(vt[i]);
		Normalize3D(vertices[i]);
		Normalize3D(vn[i]);
	}

	return 0;
}
