#include <iostream>
#include <fstream>
#include <vector>
#include <glm\glm.hpp>
#include <istream>
#include <cstdlib>
using namespace glm;
using namespace std;

// We have write() functions, because the streaming operator (operator<<) would 
// write the values as text and we want binary
void write(ostream &os, uint32_t value) {
	os.write(reinterpret_cast<const char *>(&value), 4);
}

void write(ostream &os, uint16_t value) {
	os.write(reinterpret_cast<const char *>(&value), 2);
}

void Line(const float x1, const float y1, const float x2, const float y2, vector<char> &buffer)
{
	// Bresenham's line algorithm
	const bool steep = (fabs(y2 - y1) > fabs(x2 - x1));
	if (steep)
	{
		swap(x1, y1);
		swap(x2, y2);
	}

	if (x1 > x2)
	{
		swap(x1, x2);
		swap(y1, y2);
	}

	const float dx = x2 - x1;
	const float dy = fabs(y2 - y1);

	float error = dx / 2.0f;
	const int ystep = (y1 < y2) ? 1 : -1;
	int y = (int)y1;

	const int maxX = (int)x2;

	for (int x = (int)x1; x<maxX; x++)
	{
		if (steep)
		{
			set_black_point(buffer, y, x);
		}
		else
		{
			set_black_point(buffer, x, y);
		}
		error -= dy;
		if (error < 0)
		{
			y += ystep;
			error += dx;
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

void set_black_point(vector<char> &buffer, int x, int y) {
	// We have to start from the end because BMP files are upside-down
	int index = ((511 - y) * 512 + x) * 3;
	buffer[index] = 0;
	buffer[index + 1] = 0;
	buffer[index + 2] = 0;
}

int main() {
	// Allocate a pixel buffer of 3 bytes per pixel (red, blue, green)
	vector<char> pixel_buffer(512 * 512 * 3, 0xff);

	// Import vertrices from file
	ifstream file;
	file.open("Coordinates.txt", ios::in);  // Opening the coordinates
	vector<vec2> data;
	for (int i = 0;; ++i) {
		file >> data[i].x >> data[i].y;
		if (file.eof()) break;
	}
	// Draw a line between each vertrices of each face
	for (unsigned int i = 0; i < data.size(); i += 3) {
		vec2 v1 = data[i];
		vec2 v2 = data[i + 1];
		vec2 v3 = data[i + 2];
		Line(v1.x, v1.y, v2.x, v2.y, pixel_buffer);
		Line(v2.x, v2.y, v3.x, v3.y, pixel_buffer);
		Line(v1.x, v1.y, v3.x, v3.y, pixel_buffer);
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
