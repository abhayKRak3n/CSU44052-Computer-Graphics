#ifndef MESH_HEADER
#define MESH_HEADER
#pragma region INCLUDES 
// Windows includes (For Time, IO, etc.)
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector> // STL dynamic memory.
// OpenGL includes
#include <GL/glew.h>
#include <GL/freeglut.h>
// Assimp includes
#include <assimp/cimport.h> // scene importer
#include <assimp/scene.h> // collects data
#include <assimp/postprocess.h> // various extra operations
// Project includes
#include "maths_funcs.h"
#pragma endregion INCLUDES 

#pragma region SimpleTypes
typedef struct
{
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
	//std::vector<vec3> mTangents;
} ModelData1;
#pragma endregion SimpleTypes


class Mesh {
public:
	Mesh();
	ModelData1 load_mesh(const char* filename);
};

#endif