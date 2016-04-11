#include <vector>
#include <cstdio>
#include <stdlib.h>
#include <cstdlib>
#include <cstring>
#include <glm\glm.hpp>
#include <istream>
#include <iostream>
#include <fstream>
#include <main.h>
#include <algorithm>
#include <glm/gtx/projection.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/glew.h>

using namespace glm;
using namespace std;


/* An OBJ file is composed of a list of vertices (v), of textures (vt), normals (vn) and faces (f).
 Vertices defined as a x, y, z : v	0.1		3.13	-7.9
 Texture coordinates defined as a s, t in the range[0:1] : vt		0.2		0.76
 Normals defined as a x, y, z : vn	0.2		-0.3	0.6
 Face defined as a references of vertices, textures, and normals. 
 Examples of faces :	f v1			v2			v3
  						f v1/vt1		v2/vt2		v3/vt3
						f v1/vt1/vn1	v2/vt2/vn2	v3/vt3/vn3 
						f v1//vn1		v2//vn2		v3//vn3
 In the face definition elements are indexed and indexes start at 1 but can also be negative (-1 is the last vertice).


We will need the OpenGL library GLM.
*/

// Projects a vec3 using the perspective function, we tried using this function to project our vertices but it doesn't correspond to a specific plane so we had to do it manually
/* 
vec3 Perspectiveproj(vec3 tempv) {
	mat4 projectionMatrix = perspective(60.0f, (float)Console::WindowWidth / (float)Console::WindowHeight, 0.1f, 100.f);
	vec4 transformed(tempv, 1.0);
	transformed = projectionMatrix * transformed;
	vec3 final(transformed);
	return final;
}
*/
// Projects a vector to the plane normal to (1,1,1) using a projection matrix and then changing the base to (u,v,w) with u : 1/sqrt(6) {1,1,-2}; v : 1/sqrt(2) {-1,1,0}; w : 1/2*sqrt(3) {-1,1,0}
vec3 Projection(vec3 tempv) {
	mat3 proj = { { 2.0/3, -1.0/3 , -1.0/3 },{ -1.0/3, 2.0/3 , -1.0/3 } ,{ -1.0/3, -1.0/3 , 2.0/3 } }; // The change of basis matrix
	vec3 plane = { 1.0, 1.0, 1.0 }; // Our plane matrix
	vec3 finalv = transpose(proj) * (tempv-plane); // Vprojected = tM * (V - P)
	return finalv;
}

// Extract the x and y coordinate from a vec3 vector
vec2 Projection2D(vec3 tempv) {
	vec2 v2d(tempv.x,tempv.y);
	return v2d;
}

// Writes all the vec2 coordinates in a specific file
void writeinfile(const char* path, vector<vec2> tempv) {
	ofstream file(path, ios::out); // Opens the file
	if (file) {
		for (unsigned int i = 0; i < tempv.size(); i++) {
			file << tempv[i].x << " " << tempv[i].y << endl; // Writes one vec2 per line in the following format : "X Y"
		}
	}
	else cerr << "Can't open file! " << endl; // If the file can't be opened => error message
}

// Load all the vertices, textures and normals in vectors of vec2 and vec3, indexed accordingly to faces
bool loadOBJ(const char * path, vector <vec3> &out_v, vector <vec2> &out_vt, vector <vec3> &out_vn) {

	// We are going to use vec2 and vec3 to store the coordinates of these elements. They are vector classes that can store 2/3 coordinates

	vector <unsigned int> vIndices, vtIndices, vnIndices;
	vector <vec3> temp_v; // Vertice coordinates
	vector <vec2> temp_vt; // Texture coordinates
	vector <vec3> temp_vn; // Normals coordinates

	// We need to open the OBJ file using fopen ("rb" parameter is for non txt files)

	FILE *file = fopen(path, "rb");

	// If file is empty then error message
	if (file == NULL) {
		printf("Impossible to open the file !");
		return false;
	}

	
	while (1) {
		char lineHeader1[128]; // Will store the whole line to see the format of the face (v v v; v/vt v/vt v/vt; v/vt/vn v/vt/vn v/vt/vn; v//vn v//vn v//vn)
		char lineHeader[128]; // Will store the first character to identify if it is a v, vt, vn, or f
		fpos_t pos; // Will store the position index in the file to go back to the beginning o the line after storing it
		// Store the current position in the file
		if (fgetpos(file, &pos) != 0) //If there is an error in the current position in the file
		{
			printf("Error position");
		}
		// Read the line in order to see the format of the face
		fgets(lineHeader1, 100, file);

		string s = string(lineHeader1); // Converts it to a string, easier to analyze

		// Reset the position in the file
		int test = fsetpos(file, &pos);

		// Read the first character
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF) // Exit the loop if it is the end of the file
			break;

		// We need to check whether the element is v, vt, vn or f using string comparator strcmp
		// Then we store all the vertices, textures and normals in vec2 and vec3
		if (strcmp(lineHeader, "v") == 0) // Checks if the first element is a v
		{
			vec3 vertex;
			fscanf_s(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z); // Stores the coordinates of vertices
			temp_v.push_back(vertex); // Store all the vertices
		}
		else if (strcmp(lineHeader, "vt") == 0) {
			vec2 vt;
			fscanf_s(file, "%f %f\n", &vt.x, &vt.y); // Stores the coordinates of textures
			temp_vt.push_back(vt);
		}
		else if (strcmp(lineHeader, "vn") == 0) {
			vec3 vn;
			fscanf_s(file, "%f %f %f\n", &vn.x, &vn.y, &vn.z); // Stores the coordinates of normals
			temp_vn.push_back(vn);
		}
		// We will need to identify the format of the faces 
		// Exctracts all the indices for each face
		else if (strcmp(lineHeader, "f") == 0) {
			// Check the format of f, through the number of "/"
			size_t n = count(s.begin(), s.end(), '/'); 
			if (n == 0) // If 0 "/" then the format is v v v 
			{
				unsigned int vIndex[3];
				int matches = fscanf_s(file, "%d %d %d\n", &vIndex[0], &vIndex[1], &vIndex[2]); // Faces give indices of vertices, so we'll store all the indices of the vertices
				if (matches != 3) {
					printf("File can't be read.\n");
					return false;
				}
				vIndices.push_back(vIndex[0]); // Store all the indices in a vector
				vIndices.push_back(vIndex[1]);
				vIndices.push_back(vIndex[2]);
				vtIndices.push_back(NULL);
				vtIndices.push_back(NULL);
				vtIndices.push_back(NULL);
				vnIndices.push_back(NULL);
				vnIndices.push_back(NULL);
				vnIndices.push_back(NULL);
			}
			if (n == 3) // If 3 "/" then the format is v/vt v/vt v/vt
			{
				unsigned int vIndex[3], vtIndex[3];
				int matches = fscanf_s(file, "%d/%d %d/%d %d/%d\n", &vIndex[0], &vtIndex[0], &vIndex[1], &vtIndex[1], &vIndex[2], &vtIndex[2]);
				if (matches != 6) {
					printf("File can't be read.\n");
					return false;
				}
				vIndices.push_back(vIndex[0]);
				vIndices.push_back(vIndex[1]);
				vIndices.push_back(vIndex[2]);
				vtIndices.push_back(vtIndex[0]);
				vtIndices.push_back(vtIndex[1]);
				vtIndices.push_back(vtIndex[2]);
				vnIndices.push_back(NULL);
				vnIndices.push_back(NULL);
				vnIndices.push_back(NULL);
			}
			if (n == 6) // If there is 6 "/" then there are 2 possibilities : v/vt/vn or v//vn
			{
				unsigned int vIndex[3], vtIndex[3], vnIndex[3];
				test = fgetpos(file, &pos); // We will need to get back at the beginning of the line to read again if it is the wrong format
				// We will first check if it finds 9 elements, in this case it will be v/vt/vn
				int matches = fscanf_s(file, "%d/%d/%d %d/%d/%d %d/%d/%d\n", &vIndex[0], &vtIndex[0], &vnIndex[0], &vIndex[1], &vtIndex[1], &vnIndex[1], &vIndex[2], &vtIndex[2], &vnIndex[2]);
				// Two possibilities here, v//vn or v/vt/vn
				if (matches != 9) // If there isn't 9 elements then it is v//vn but we will need to rewing to the beginning of the line to read again the indices
				{
					test = fsetpos(file, &pos); // Restarts from the beginning of the line
					int matches = fscanf_s(file, "%d//%d %d//%d %d//%d\n", &vIndex[0], &vnIndex[0], &vIndex[1], &vnIndex[1], &vIndex[2], &vnIndex[2]);
					if (matches != 6) {
						printf("File can't be read.\n");
						return false;
					} // If the format is v//vn 
					vIndices.push_back(vIndex[0]);
					vIndices.push_back(vIndex[1]);
					vIndices.push_back(vIndex[2]);
					vtIndices.push_back(NULL);
					vtIndices.push_back(NULL);
					vtIndices.push_back(NULL);
					vnIndices.push_back(vnIndex[0]);
					vnIndices.push_back(vnIndex[1]);
					vnIndices.push_back(vnIndex[2]);
				} // If the format is v/vt/vn
				else if (matches == 9) {
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
				else if (matches != 9) {
					printf("File can't be read.\n");
					return false;
				}

			}

		}

		/* Select the corresponding vertice, texture, normal and store it in a vector such that for index 1/2/3 we have the first face etc...
		For example is a face is 1/4/7, then we will put the vertices in the following order in the vector : {vertice 1, vertice 4, vertice 7}
		*/
		for (unsigned int i = 0; i < vIndices.size(); i++) {
			unsigned int vIndex = vIndices[i]; // We get the indices we found in faces
			vec3 vertex = temp_v[vIndex - 1]; // Face indexing starts at 1, whereas vector indexing starts at 0, we get the vertice the face is referring to
			out_v.push_back(vertex);
		}
		if (temp_vt.size() != 0) {
			for (unsigned int i = 0; i < vtIndices.size(); i++) {
				unsigned int vtIndex = vtIndices[i];
				vec2 vt = temp_vt[vtIndex - 1];
				out_vt.push_back(vt);
			}
		}
		if (temp_vn.size() != 0) {
			for (unsigned int i = 0; i < vnIndices.size(); i++) {
				unsigned int vnIndex = vnIndices[i];
				vec3 vn = temp_vn[vnIndex - 1];
				out_vn.push_back(vn);
			}
		}
		}
}

// We have write() functions, because the streaming operator (operator<<) would 
// write the values as text and we want binary
void write(ostream &os, uint32_t value) {
	os.write(reinterpret_cast<const char *>(&value), 4);
}

void write(ostream &os, uint16_t value) {
	os.write(reinterpret_cast<const char *>(&value), 2);
}

void set_black_point(vector<char> &buffer, int x, int y) {
	// We have to start from the end because BMP files are upside-down
	int index = ((511 - y) * 512 + x) * 3;
	buffer[index] = 0;
	buffer[index + 1] = 0;
	
	buffer[index + 2] = 0;
}

// Bresenham algorithm helps color pixels to draw line between 2 coordinates
void Bresenham(vector<char> &buffer, 
	int x1,
	int y1,
	int const x2,
	int const y2)
{
	int delta_x(x2 - x1);
	// if x1 == x2, then it does not matter what we set here
	signed char const ix((delta_x > 0) - (delta_x < 0));
	delta_x = std::abs(delta_x) << 1;

	int delta_y(y2 - y1);
	// if y1 == y2, then it does not matter what we set here
	signed char const iy((delta_y > 0) - (delta_y < 0));
	delta_y = std::abs(delta_y) << 1;

	set_black_point(buffer,x1, y1);

	if (delta_x >= delta_y)
	{
		// error may go below zero
		int error(delta_y - (delta_x >> 1));

		while (x1 != x2)
		{
			if ((error >= 0) && (error || (ix > 0)))
			{
				error -= delta_x;
				y1 += iy;
			}
			// else do nothing

			error += delta_y;
			x1 += ix;

			set_black_point(buffer,x1, y1);
		}
	}
	else
	{
		// error may go below zero
		int error(delta_x - (delta_y >> 1));

		while (y1 != y2)
		{
			if ((error >= 0) && (error || (iy > 0)))
			{
				error -= delta_y;
				x1 += ix;
			}
			// else do nothing

			error += delta_x;
			y1 += iy;

			set_black_point(buffer,x1, y1);
		}
	}
}

// Write the BMP header. See the wikipedia page for more details:
void write_bmp_header(ostream &os) {
	// BMP Header
	os << "BM";							// Signature
	write(os, 512u * 512u * 3u + 54u);	// File size
	write(os, 0u);						// Reserved
	write(os, 54u);						// Image offset

										// DIB Header
	write(os, 40u);						// DIB header size
	write(os, 512u);					// Width
	write(os, 512u);					// Height
	write(os, uint16_t(1));		// Number of color planes
	write(os, uint16_t(24));		// Bits per pixel (24 = 3 bytes)
	write(os, 0u);						// Compression method
	write(os, 512u * 512u * 3u);		// Image size (pixel data)
	write(os, 0u);						// Image resolution, horizontal
	write(os, 0u);						// Image resolution, vertical
	write(os, 0u);						// Size of palette plane
	write(os, 0u);						// All colors are important
}

// Another attempt to use a built-in projection function, but we didn't manage to use it correctly
vec3 projection_glm(vec3 original)
{
	mat4 model = lookAt(vec3(1, 1, 1), vec3(0, 0, 0), vec3(0, 1, 0));;
	mat4 projection = frustum(-1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 100.0f);
	vec4 viewport(0.0f, 0.0f, 512.0f, 512.0f);

	vec3 projected = project(original, model, projection, viewport);
	return projected;

}

int main(){
	vector< vec3 > vertices;
	vector< vec2 > vt;
	vector< vec3 > vn; 
	vector< vec3 > projectedv;
	vector< vec2 > v2d;

	// Works with the given obj files except for the cube because of a nan(ind) error

	bool res = loadOBJ("diamond.obj", vertices, vt, vn); // Loads all vertices, textures and normals in the order of faces
	
	for (unsigned int i = 0; i < vertices.size(); i++) {
		vec3 tempvertices = normalize(vertices[i]); // Normalizes all the vertices
		if (tempvertices.x != tempvertices.x) {
			printf("Error in normalization : nan(ind)");
			break;
		}
		projectedv.push_back(projection_glm(tempvertices)); // Projects using the lookAt function, we couldn't find the right projection matrix so we had to use a built-in function
		v2d.push_back(Projection2D(projectedv[i])); // Selects only the x and y coordinates of the vec3
	}

	ofstream Coordinates("Coordinates.txt", ios::out | ios::trunc);  // Creating the output file
		if (Coordinates)
		{
			Coordinates.close(); 
		}
		else  cerr << "Can't open file!" << endl;

	writeinfile("Coordinates.txt", v2d); // Writes vertice's coordinate, one vertice per line

	// Allocate a pixel buffer of 3 bytes per pixel (red, blue, green)
	vector<char> pixel_buffer(512 * 512 * 3, 0xff);

	// Import vertrices from file
	ifstream file;
	file.open("Coordinates.txt", ios::in);  // Opening the coordinates
	vector<vec2> data;
	// Retrieve coordinates in data vector
	float X, Y;
	for (int i = 0;; ++i) {
		file >> X >> Y; // Gets back x and y coordinate of the vertice
		data.push_back(vec2(X, Y)); // Store it in a vector <vec2>
		if (file.eof()) break;
	}
	// Draw a line between each vertrices of each face
	// Each face has 3 vertices, so each face draws a triangle (3 lines)
	for (unsigned int i = 0; i < data.size()-1; i += 3) {
		vec2 v1 = data[i]; // We read 3 vertices corresponding to a face
		vec2 v2 = data[i + 1];
		vec2 v3 = data[i + 2];
		Bresenham(pixel_buffer, v1.x, v1.y, v2.x, v2.y); // Given that our coordinates are normalized and in a cube of length one we have to multiply them by the frame of the bmp file (512)
		Bresenham(pixel_buffer, v1.x, v1.y, v2.x, v2.y);
		Bresenham(pixel_buffer, v1.x, v1.y, v2.x, v2.y);
	}

	// Enable throwing exceptions, and open the file as binary
	ofstream ofs;
	ofs.exceptions(ios::badbit);
	ofs.open("art.bmp", ios::binary);

	// Write header then pixel buffer
	write_bmp_header(ofs);
	ofs.write(&pixel_buffer[0], pixel_buffer.size());

	return 0;
}