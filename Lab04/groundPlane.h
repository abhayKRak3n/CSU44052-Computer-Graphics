#include "Mesh.h" 
#include "Shaders.h"
#pragma once
class GroundPlane
{
private:
	vec3 groundLocation;
	ModelData1 groundMesh;
	unsigned int vpvbo = 0;
	unsigned int vnvbo = 0;
	unsigned int vtvbo = 0;

public:
	GroundPlane();
	GroundPlane(vec3 groundLocation);
	void genGround();
	void drawGround(Shader shader, int width, int height, unsigned int diffuseMapGrnd);
};
