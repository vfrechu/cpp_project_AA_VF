#define _CRT_SECURE_NO_WARNINGS
#pragma once
#pragma warning(disable:4996)
using namespace std;
using namespace glm;
vec3 Projection(vec3 tempv);
vec2 Projection2D(vec3 tempv);
void set_black_point(vector<char> &buffer, int x, int y);
void writeinfile(ofstream file, vector<vec2> tempv);
bool loadOBJ(const char * path, vector <vec3> &out_v, vector <vec2> &out_vt, vector <vec3> &out_vn);

