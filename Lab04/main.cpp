# pragma region includes 
#include <windows.h>
#include <mmsystem.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <assimp/cimport.h> 
#include <assimp/scene.h> 
#include <assimp/postprocess.h> 
#include "maths_funcs.h"
#include "Model.h"
#include "Shaders.h"
#include "Camera.h"
#include "Object.h"
#include "Shaders.h"
//#include "GroundPlane.h"
#include "Skybox.h"
#define STB_IMAGE_IMPLEMENTATION    
#include "stb_image.h"
using namespace std;
#pragma endregion includes

GLfloat x,/*tr,t,r=.032,*/i = -45;
#define c (3.141/180)
//shaders
int width = 800;
int height = 600;
Shader skyboxShader;
Shader objectShader;
Shader baseShader;

//skybox
skybox sky;
unsigned int skyBoxTextureMap;

float ambient = 0.5f;
float diffuse = 0.4f;
float spec = 1.0f;

vec3 lightPositions[] = {
	vec3(0.0f, 8.0f, -15.0f),
	vec3(15.0f, 6.0f, 10.0f),
	vec3(-10.0f, 3.0f, -5.0f)
};

Camera camera = Camera(vec3(0.0f, 1.0f, 0.0f), vec3(0.0f, 0.0f, 1.0f));

GLfloat rotate_increment = 1.0f;
GLfloat translate_increment = 0.1f;
GLfloat scale_increment = 2.0f;

GLfloat perspective_fovy = 45.0f;

boolean animation = true;
boolean mouseInput = false;

int matrix_location, view_mat_location, proj_mat_location;
int object_color_location, lights_position_location;
int view_pos_location, specular_coef_location;
mat4 view, persp_proj;
GLuint diffuseShaderProgramID, specularShaderProgramID, textureShaderProgramID;

//ground plane
const int NUM_GROUNDS = 9;
//GroundPlane grounds[NUM_GROUNDS];
vec3 groundLocations[NUM_GROUNDS] = { vec3(-1000.0f, -20.0f, -1000.0f), vec3(0.0f, -20.0f, -1000.0f),vec3(1000.0f, -20.0f, -1000.0f),
									  vec3(-1000.0f, -20.0f, 0.0f), vec3(0.0f, -20.0f, 0.0f), vec3(1000.0f, -20.0f, 0.0f),
									  vec3(-1000.0f, -20.0f, 1000.0f), vec3(0.0f, -20.0f, 1000.0f), vec3(1000.0f, -20.0f, 1000.0f) };

//textures
unsigned int diffuseMapShark;
unsigned int diffuseMapGround;

//lights
vec3 lightPos(0.0f, 100.0f, 2.0f);
vec3 pointLightPositions[] = {
	  vec3(0.0f,  0.2f,  2.0f),vec3(200.0f, -3.3f, -4.0f),vec3(-200.0f,  2.0f, -12.0f),vec3(0.0f,  0.0f, -3.0f),
	  vec3(0.0f,  0.2f,  400.0f),vec3(200.0f, -3.3f, 400.0f),vec3(-200.0f,  2.0f, 400.0f),vec3(0.0f,  0.0f, 400.0f),
	  vec3(0.0f,  0.2f,  -400.0f),vec3(200.0f, -3.3f, -400.0f),vec3(-200.0f,  2.0f, -12.0f),vec3(0.0f,  0.0f, -400.0f),

};

vec3 redLight = vec3(1.0f, 0.0f, 0.0f);
vec3 blueLight = vec3(0.0f, 0.0f, 1.0f);
vec3 greenLight = vec3(0.0f, 1.0f, 0.0f);
vec3 normalLight = vec3(0.3f, 0.3f, 0.3f);
vec3 currentLight;

vec3 currentBrightness;
vec3 lowBrightness = vec3(0.0075f, 0.0075f, 0.0075f);
vec3 midBrightness = vec3(0.075f, 0.075f, 0.075f);
vec3 highBrightness = vec3(0.75f, 0.75f, 0.75f);


/*----------------------------------------------------------------------------
MESH TO LOAD
----------------------------------------------------------------------------*/
// this mesh is a dae file format but you should be able to use any other format too, obj is typically what is used
// put the mesh in your project directory, or provide a filepath for it here
#define MESH_NAME "Shark.dae"
/*----------------------------------------------------------------------------
---------------------------------------------------------------------------*/
/*
#pragma region SimpleTypes
typedef struct
{
	size_t mPointCount = 0;
	std::vector<vec3> mVertices;
	std::vector<vec3> mNormals;
	std::vector<vec2> mTextureCoords;
} ModelData;

#pragma endregion SimpleTypes
*/
using namespace std;
GLuint shaderProgramID;

ModelData mesh_data;
unsigned int mesh_vao = 0;

GLuint loc1, loc2, loc3;
GLfloat rotate_y = 0.0f;


#pragma region MESH LOADING
/*----------------------------------------------------------------------------
MESH LOADING FUNCTION
----------------------------------------------------------------------------*/

ModelData load_mesh(const char* file_name) {
	ModelData modelData;

	/* Use assimp to read the model file, forcing it to be read as    */
	/* triangles. The second flag (aiProcess_PreTransformVertices) is */
	/* relevant if there are multiple meshes in the model file that   */
	/* are offset from the origin. This is pre-transform them so      */
	/* they're in the right position.                                 */
	const aiScene* scene = aiImportFile(
		file_name, 
		aiProcess_Triangulate | aiProcess_PreTransformVertices
	); 

	if (!scene) {
		fprintf(stderr, "ERROR: reading mesh %s\n", file_name);
		return modelData;
	}

	printf("  %i materials\n", scene->mNumMaterials);
	printf("  %i meshes\n", scene->mNumMeshes);
	printf("  %i textures\n", scene->mNumTextures);

	for (unsigned int m_i = 0; m_i < scene->mNumMeshes; m_i++) {
		const aiMesh* mesh = scene->mMeshes[m_i];
		printf("    %i vertices in mesh\n", mesh->mNumVertices);
		modelData.mPointCount += mesh->mNumVertices;
		for (unsigned int v_i = 0; v_i < mesh->mNumVertices; v_i++) {
			if (mesh->HasPositions()) {
				const aiVector3D* vp = &(mesh->mVertices[v_i]);
				modelData.mVertices.push_back(vec3(vp->x, vp->y, vp->z));
			}
			if (mesh->HasNormals()) {
				const aiVector3D* vn = &(mesh->mNormals[v_i]);
				modelData.mNormals.push_back(vec3(vn->x, vn->y, vn->z));
			}
			if (mesh->HasTextureCoords(0)) {
				const aiVector3D* vt = &(mesh->mTextureCoords[0][v_i]);
				modelData.mTextureCoords.push_back(vec2(vt->x, vt->y));
			}
			if (mesh->HasTangentsAndBitangents()) {
				/* You can extract tangents and bitangents here              */
				/* Note that you might need to make Assimp generate this     */
				/* data for you. Take a look at the flags that aiImportFile  */
				/* can take.                                                 */
			}
		}
	}

	aiReleaseImport(scene);
	return modelData;
}

#pragma endregion MESH LOADING

// Shader Functions- click on + to expand
#pragma region SHADER_FUNCTIONS
char* readShaderSource(const char* shaderFile) {
	FILE* fp;
	fopen_s(&fp, shaderFile, "rb");

	if (fp == NULL) { return NULL; }

	fseek(fp, 0L, SEEK_END);
	long size = ftell(fp);

	fseek(fp, 0L, SEEK_SET);
	char* buf = new char[size + 1];
	fread(buf, 1, size, fp);
	buf[size] = '\0';

	fclose(fp);

	return buf;
}


static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
	// create a shader object
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		std::cerr << "Error creating shader..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	const char* pShaderSource = readShaderSource(pShaderText);

	// Bind the source code to the shader, this happens before compilation
	glShaderSource(ShaderObj, 1, (const GLchar**)&pShaderSource, NULL);
	// compile the shader and check for errors
	glCompileShader(ShaderObj);
	GLint success;
	// check for shader related errors using glGetShaderiv
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024] = { '\0' };
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		std::cerr << "Error compiling "
			<< (ShaderType == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader program: " << InfoLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Attach the compiled shader object to the program object
	glAttachShader(ShaderProgram, ShaderObj);
}

GLuint CompileShaders()
{
	//Start the process of setting up our shaders by creating a program ID
	//Note: we will link all the shaders together into this ID
	shaderProgramID = glCreateProgram();
	if (shaderProgramID == 0) {
		std::cerr << "Error creating shader program..." << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// Create two shader objects, one for the vertex, and one for the fragment shader
	AddShader(shaderProgramID, "simpleVertexShader.txt", GL_VERTEX_SHADER);
	AddShader(shaderProgramID, "simpleFragmentShader.txt", GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { '\0' };
	// After compiling all shader objects and attaching them to the program, we can finally link it
	glLinkProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Error linking shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}

	// program has been successfully linked but needs to be validated to check whether the program can execute given the current pipeline state
	glValidateProgram(shaderProgramID);
	// check for program related errors using glGetProgramiv
	glGetProgramiv(shaderProgramID, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(shaderProgramID, sizeof(ErrorLog), NULL, ErrorLog);
		std::cerr << "Invalid shader program: " << ErrorLog << std::endl;
		std::cerr << "Press enter/return to exit..." << std::endl;
		std::cin.get();
		exit(1);
	}
	// Finally, use the linked shader program
	// Note: this program will stay in effect for all draw calls until you replace it with another or explicitly disable its use
	glUseProgram(shaderProgramID);
	return shaderProgramID;
}
#pragma endregion SHADER_FUNCTIONS

// VBO Functions - click on + to expand
#pragma region VBO_FUNCTIONS
void generateObjectBufferMesh() {
	/*----------------------------------------------------------------------------
	LOAD MESH HERE AND COPY INTO BUFFERS
	----------------------------------------------------------------------------*/

	//Note: you may get an error "vector subscript out of range" if you are using this code for a mesh that doesnt have positions and normals
	//Might be an idea to do a check for that before generating and binding the buffer.

	mesh_data = load_mesh(MESH_NAME);
	unsigned int vp_vbo = 0;
	loc1 = glGetAttribLocation(shaderProgramID, "vertex_position");
	loc2 = glGetAttribLocation(shaderProgramID, "vertex_normal");
	loc3 = glGetAttribLocation(shaderProgramID, "vertex_texture");

	glGenBuffers(1, &vp_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mVertices[0], GL_STATIC_DRAW);
	unsigned int vn_vbo = 0;
	glGenBuffers(1, &vn_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glBufferData(GL_ARRAY_BUFFER, mesh_data.mPointCount * sizeof(vec3), &mesh_data.mNormals[0], GL_STATIC_DRAW);

	//	This is for texture coordinates which you don't currently need, so I have commented it out
	//	unsigned int vt_vbo = 0;
	//	glGenBuffers (1, &vt_vbo);
	//	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
	//	glBufferData (GL_ARRAY_BUFFER, monkey_head_data.mTextureCoords * sizeof (vec2), &monkey_head_data.mTextureCoords[0], GL_STATIC_DRAW);

	unsigned int vao = 0;
	glBindVertexArray(vao);

	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vp_vbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vn_vbo);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	//	This is for texture coordinates which you don't currently need, so I have commented it out
	//	glEnableVertexAttribArray (loc3);
	//	glBindBuffer (GL_ARRAY_BUFFER, vt_vbo);
	//	glVertexAttribPointer (loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);
}
#pragma endregion VBO_FUNCTIONS


#pragma region SET_UP_FUNCTIONS
unsigned int loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void setupTextures() {
	string path = "textures/sharkTexture.jpg";
	diffuseMapShark = loadTexture(path.c_str());
}

unsigned int loadSkyTexture()
{
	std::string faces[] =
	{
		"skybox/uw_bk.jpg",
		"skybox/uw_dn.jpg",
		"skybox/uw_ft.jpg",
		"skybox/uw_lf.jpg",
		"skybox/uw_rt.jpg",
	   "skybox/uw_up.jpg"
	};

	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < 6; i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;

}

void setUpSkyBox() {
	//set up skybox 
	sky = skybox();
	sky.genSkybox();
	skyBoxTextureMap = loadSkyTexture();
	skyboxShader.use();
	skyboxShader.setInt("skybox", 0);
	objectShader.use();
}

void generateGround() {

	/*for (int i = 0; i < NUM_GROUNDS; i++) {
		grounds[i] = GroundPlane(groundLocations[i]);
		grounds[i].genGround();
	}*/
}

void setupShaders() {
	objectShader= Shader("mainVS.txt", "mainFS.txt");
	skyboxShader = Shader("skyboxVS.txt", "skyboxFS.txt");
}

void setUpLights() {
	//set light properties of objects 
	  // directional light
	objectShader.setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	objectShader.setVec3("dirLight.ambient", currentBrightness);
	objectShader.setVec3("dirLight.diffuse", currentLight); 
	objectShader.setVec3("dirLight.specular", 0.75f, 0.75f, 0.75f); 

	// spotLight
	objectShader.setVec3("spotLight.position", camera.position);
	objectShader.setVec3("spotLight.direction", camera.direction);
	objectShader.setVec3("spotLight.ambient", 20.0f, 20.0f, 20.0f); 
	objectShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
	objectShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
	objectShader.setFloat("spotLight.constant", 1.0f);
	objectShader.setFloat("spotLight.linear", 0.001);
	objectShader.setFloat("spotLight.quadratic", 0.005);
	objectShader.setFloat("spotLight.cutOff", cos((12.5f * 3.14f) / 180)); //rad = degree * (pi / 180
	objectShader.setFloat("spotLight.outerCutOff", cos((15.0f * 3.14f) / 180));

	// point light 1
	objectShader.setVec3("pointLights[0].position", pointLightPositions[0]);
	objectShader.setVec3("pointLights[0].ambient", 5.0f, 5.0f, 5.0f); 
	objectShader.setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	objectShader.setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	objectShader.setFloat("pointLights[0].constant", 1.0f);
	objectShader.setFloat("pointLights[0].linear", 0.001); 
	objectShader.setFloat("pointLights[0].quadratic", 0.05); 
	// point light 2
	objectShader.setVec3("pointLights[1].position", pointLightPositions[1]);
	objectShader.setVec3("pointLights[1].ambient", 10.0f, 10.0f, 10.0f);
	objectShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	objectShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	objectShader.setFloat("pointLights[1].constant", 1.0f);
	objectShader.setFloat("pointLights[1].linear", 0.001);
	objectShader.setFloat("pointLights[1].quadratic", 0.005);
	// point light 3
	objectShader.setVec3("pointLights[2].position", pointLightPositions[2]);
	objectShader.setVec3("pointLights[2].ambient", 10.0f, 10.0f, 10.0f);
	objectShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	objectShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	objectShader.setFloat("pointLights[2].constant", 1.0f);
	objectShader.setFloat("pointLights[2].linear", 0.001);
	objectShader.setFloat("pointLights[2].quadratic", 0.005);
	// point light 4
	objectShader.setVec3("pointLights[3].position", pointLightPositions[3]);
	objectShader.setVec3("pointLights[3].ambient", 20.0f, 20.0f, 20.0f); 
	objectShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	objectShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	objectShader.setFloat("pointLights[3].constant", 1.0f);
	objectShader.setFloat("pointLights[3].linear", 0.001); 
	objectShader.setFloat("pointLights[3].quadratic", 0.005); 
	// point light 5
	objectShader.setVec3("pointLights[4].position", pointLightPositions[4]);
	objectShader.setVec3("pointLights[4].ambient", 10.0f, 10.0f, 10.0f);//0.05
	objectShader.setVec3("pointLights[4].diffuse", 0.8f, 0.8f, 0.8f);
	objectShader.setVec3("pointLights[4].specular", 1.0f, 1.0f, 1.0f);
	objectShader.setFloat("pointLights[4].constant", 1.0f);
	objectShader.setFloat("pointLights[4].linear", 0.001);
	objectShader.setFloat("pointLights[4].quadratic", 0.005);
	// point light 6
	objectShader.setVec3("pointLights[5].position", pointLightPositions[5]);
	objectShader.setVec3("pointLights[5].ambient", 10.0f, 10.0f, 10.0f);
	objectShader.setVec3("pointLights[5].diffuse", 0.8f, 0.8f, 0.8f);
	objectShader.setVec3("pointLights[5].specular", 1.0f, 1.0f, 1.0f);
	objectShader.setFloat("pointLights[5].constant", 1.0f);
	objectShader.setFloat("pointLights[5].linear", 0.001);
	objectShader.setFloat("pointLights[5].quadratic", 0.005);
	// point light 7
	objectShader.setVec3("pointLights[6].position", pointLightPositions[6]);
	objectShader.setVec3("pointLights[6].ambient", 20.0f, 20.0f, 20.0f); //0.05
	objectShader.setVec3("pointLights[6].diffuse", 0.8f, 0.8f, 0.8f);
	objectShader.setVec3("pointLights[6].specular", 1.0f, 1.0f, 1.0f);
	objectShader.setFloat("pointLights[6].constant", 1.0f);
	objectShader.setFloat("pointLights[6].linear", 0.001); //0.09 smaller attenuation bigger light 
	objectShader.setFloat("pointLights[6].quadratic", 0.005); //0.032
	// point light 8
	objectShader.setVec3("pointLights[7].position", pointLightPositions[7]);
	objectShader.setVec3("pointLights[7].ambient", 10.0f, 10.0f, 10.0f);//0.05
	objectShader.setVec3("pointLights[7].diffuse", 0.8f, 0.8f, 0.8f);
	objectShader.setVec3("pointLights[7].specular", 1.0f, 1.0f, 1.0f);
	objectShader.setFloat("pointLights[7].constant", 1.0f);
	objectShader.setFloat("pointLights[7].linear", 0.001);
	objectShader.setFloat("pointLights[7].quadratic", 0.005);
	// point light 9
	objectShader.setVec3("pointLights[8].position", pointLightPositions[8]);
	objectShader.setVec3("pointLights[8].ambient", 10.0f, 10.0f, 10.0f);
	objectShader.setVec3("pointLights[8].diffuse", 0.8f, 0.8f, 0.8f);
	objectShader.setVec3("pointLights[8].specular", 1.0f, 1.0f, 1.0f);
	objectShader.setFloat("pointLights[8].constant", 1.0f);
	objectShader.setFloat("pointLights[8].linear", 0.001);
	objectShader.setFloat("pointLights[8].quadratic", 0.005);
}

#pragma endregion SET_UP_FUNCTIONS

void display() {

	// tell GL to only draw onto a pixel if the shape is closer to the viewer
	glEnable(GL_DEPTH_TEST); // enable depth-testing
	glDepthFunc(GL_LESS); // depth-testing interprets a smaller value as "closer"
	glClearColor(0.50f, 0.50f, 0.50f, 0.5f);		//  0.5f, 0.5f, 0.5f, 1.0f ; 0.0f, 0.0f, 0.0f, 1.0f ; 0.50f, 0.50f, 0.50f, 0.5f
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(shaderProgramID);
	mat4 projection = perspective(45.0f, (float)width / (float)height, 0.1f, 1000.0f);
	glColor3f(1, 1, 1);
	glPointSize(5);
	
	// Camera setup
	persp_proj = perspective(perspective_fovy, (float)width / (float)height, 0.1f, 1000.0f);
	view = look_at(
		camera.position,
		camera.position + camera.direction,
		camera.up
	);
	objectShader.use();
	objectShader.setInt("material.diffuse", 0);
	objectShader.setInt("material.specular", 1);
	objectShader.setFloat("material.shininess", 62.0f);
	objectShader.setVec3("viewPos", camera.position);
	objectShader.setMat4("projection", projection);
	objectShader.setMat4("view", view);
	setUpLights();
	/*for (int i = 0; i < NUM_GROUNDS; i++) {
		grounds[i].drawGround(objectShader, width, height, diffuseMapGround);
	}*/

	//Declare your uniform variables that will be used in your shader
	int matrix_location = glGetUniformLocation(shaderProgramID, "model");
	int view_mat_location = glGetUniformLocation(shaderProgramID, "view");
	int proj_mat_location = glGetUniformLocation(shaderProgramID, "proj");

	

	// Root of the Hierarchy
	mat4 model = identity_mat4();
	model = rotate_z_deg(model, rotate_y);
	view = translate(view, vec3(0.0, 0.0, -10.0f));
	objectShader.setMat4("model", model);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMapShark);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, diffuseMapShark);

	// update uniforms & draw
	glUniformMatrix4fv(proj_mat_location, 1, GL_FALSE, persp_proj.m);
	glUniformMatrix4fv(view_mat_location, 1, GL_FALSE, view.m);
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, model.m);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffuseMapShark);

	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);

	
/*	glDepthMask(GL_FALSE);
	skyboxShader.use();
	// ... set view and projection matrix
	glBindVertexArray(skyboxVAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glDepthMask(GL_TRUE);
	//glm::mat4 view = glm::mat4(glm::mat3(camera.GetViewMatrix()));
*/
	
/*	// Set up the child matrix
	mat4 modelChild = identity_mat4();
	modelChild = rotate_z_deg(modelChild, 180);
	modelChild = rotate_y_deg(modelChild, rotate_y);
	modelChild = translate(modelChild, vec3(0.0f, 1.9f, 0.0f));

	// Apply the root matrix to the child matrix
	modelChild = model * modelChild;

	// Update the appropriate uniform and draw the mesh again
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, modelChild.m);
	glDrawArrays(GL_TRIANGLES, 0, mesh_data.mPointCount);
*/	

	//draw skybox 
	skyboxShader.use();
	glDepthFunc(GL_LEQUAL);  
	//view = identity_mat4();
	skyboxShader.setMat4("view", view);
	skyboxShader.setMat4("projection", projection);
	sky.drawSkyBox(skyboxShader, skyBoxTextureMap);

	// sine wave code
	while (i <= 135)
	{
		x = c * i;
		glColor3f(0, 1, 0);
		glBegin(GL_QUADS);
		glVertex2f(-.9 + x, -.05 + sin(8 * x));
		glVertex2f(-1 + x, -.1 + sin(8 * x));
		glVertex2f(-1 + x, .1 + sin(8 * x));
		glVertex2f(-.9 + x, .05 + sin(8 * x));
		glEnd();

		glBegin(GL_POLYGON);
		glVertex2f(-.9 + x, .05 + sin(8 * x));
		glVertex2f(-.8 + x, .15 + sin(8 * x));
		glVertex2f(-.7 + x, .12 + sin(8 * x));
		glVertex2f(-.6 + x, 0 + sin(8 * x));
		glVertex2f(-.7 + x, -.12 + sin(8 * x));
		glVertex2f(-.8 + x, -.15 + sin(8 * x));
		glVertex2f(-.9 + x, -.05 + sin(8 * x));
		glEnd();

		glColor3f(0, 0, 0);
		glBegin(GL_LINE_STRIP);
		glVertex2f(-.75 + x, .08 + sin(8 * x));
		glVertex2f(-.8 + x, .04 + sin(8 * x));
		glVertex2f(-.75 + x, 0 + sin(8 * x));
		glEnd();

		/* glBegin(GL_POINTS);
		 for(t=45;t<=-45;t+10)
		 {
		  tr=c*t;
		  glVertex2f(r*cos(tr)+x,r*sin(tr)+sin(8*x));
		 }
		 glEnd(); */

		glFlush();
		glClear(GL_COLOR_BUFFER_BIT);
		//_sleep(100);
		i += 1;
	}

	glutSwapBuffers();
}

void updateScene() {
	static DWORD last_time = 0;
	DWORD curr_time = timeGetTime();
	if (last_time == 0)
		last_time = curr_time;
	float delta = (curr_time - last_time) * 0.001f;
	last_time = curr_time;

	glutPostRedisplay();
}

void mouseMoved(int newMouseX, int newMouseY) {
	if (mouseInput) {
		printf("MOUSE MOVED - Rotate camera. \n");
		camera.rotate((GLfloat)newMouseX - width / 2, 0, 0);
		glutWarpPointer(width / 2, height / 2);
	}
}

void init()
{
	// Set up the shaders
	GLuint shaderProgramID = CompileShaders();
	// load mesh into a vertex buffer array
	generateObjectBufferMesh();
	setupShaders();
	setupTextures();
	setUpSkyBox();
	generateGround();
	currentLight = normalLight;
	currentBrightness = midBrightness;
}

void keypress(unsigned char key, int x, int y) {
	switch (key) {
	case 'W':
	case 'w':
		printf("PRESSED W - move forward.\n");
		camera.move(translate_increment, 0.0f, 0.0f);
		break;
	case 'A':
	case 'a':
		printf("PRESSED A - move left.\n");
		camera.move(0.0f, -translate_increment, 0.0f);
		break;
	case 'S':
	case 's':
		printf("PRESSED S - move backward.\n");
		camera.move(-translate_increment, 0.0f, 0.0f);
		break;
	case 'D':
	case 'd':
		printf("PRESSED D - move right.\n");
		camera.move(0.0f, translate_increment, 0.0f);
		break;
	case 'm':
	case 'M':
		printf("PRESSED M - Toggled mouse input \n");
		mouseInput = !mouseInput;
		break;
	case 'p':
	case 'P':
		printf("Switched perspective FOV. \n");
		if (perspective_fovy == 30.0f) {
			perspective_fovy = 45.0f;
		}
		else if (perspective_fovy == 45.0f) {
			perspective_fovy = 60.0f;
		}
		else {
			perspective_fovy = 30.0f;
		}
		break;
	case 'Q':
	case 'q':
		printf("PRESSED Q - QUIT. \n");
		glutLeaveMainLoop();
		break;
	case 'I':
	case 'i':
		printf("Increased brightness. \n");
		currentBrightness = highBrightness;
		break;
	case 'K':
	case 'k':
		printf("Lowered brightness. \n");
		currentBrightness = lowBrightness;
		break;
	case 'H':
	case 'h':
		printf("Lighting returned to default. \n");
		currentBrightness = midBrightness;
		currentLight = normalLight;
		break;
	case 'R':
	case 'r':
		printf("Changed to red light. \n");
		currentLight = redLight;
		break;
	case 'G':
	case 'g':
		printf("Changed to green light. \n");
		currentLight = greenLight;
		break;
	case 'B':
	case 'b':
		printf("Changed to blue light. \n");
		currentLight = blueLight;
		break;
	}
}

int main(int argc, char** argv) {

	// Set up the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(width, height);
	glutCreateWindow("Demo");

	// Tell glut where the display function is
	glutDisplayFunc(display);
	glutIdleFunc(updateScene);
	glutKeyboardFunc(keypress);
	glutPassiveMotionFunc(mouseMoved);

	// A call to glewInit() must be done after glut is initialized!
	GLenum res = glewInit();
	// Check for any errors
	if (res != GLEW_OK) {
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}
	// Set up your objects and shaders
	init();
	// Begin infinite event loop
	glutMainLoop();
	return 0;
}
