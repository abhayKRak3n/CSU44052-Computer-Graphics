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
#include "Shaders.h"
#pragma endregion INCLUDES
#pragma once
class skybox
{
private:
    unsigned int skyboxVAO, skyboxVBO;

public:
    skybox();
    void genSkybox();
    void drawSkyBox(Shader skyboxShader, unsigned int skyboxTextureMap);

};