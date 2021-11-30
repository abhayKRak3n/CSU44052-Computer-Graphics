#include "skybox.h"
//vertex data for sky box
GLfloat skyboxVertices[] = {
	// positions          
	-50.0f,  50.0f, -50.0f,
	-50.0f, -50.0f, -50.0f,
	 50.0f, -50.0f, -50.0f,
	 50.0f, -50.0f, -50.0f,
	 50.0f,  50.0f, -50.0f,
	-50.0f,  50.0f, -50.0f,

	-50.0f, -50.0f,  50.0f,
	-50.0f, -50.0f, -50.0f,
	-50.0f,  50.0f, -50.0f,
	-50.0f,  50.0f, -50.0f,
	-50.0f,  50.0f,  50.0f,
	-50.0f, -50.0f,  50.0f,

	 50.0f, -50.0f, -50.0f,
	 50.0f, -50.0f,  50.0f,
	 50.0f,  50.0f,  50.0f,
	 50.0f,  50.0f,  50.0f,
	 50.0f,  50.0f, -50.0f,
	 50.0f, -50.0f, -50.0f,

	-50.0f, -50.0f,  50.0f,
	-50.0f,  50.0f,  50.0f,
	 50.0f,  50.0f,  50.0f,
	 50.0f,  50.0f,  50.0f,
	 50.0f, -50.0f,  50.0f,
	-50.0f, -50.0f,  50.0f,

	-50.0f,  50.0f, -50.0f,
	 50.0f,  50.0f, -50.0f,
	 50.0f,  50.0f,  50.0f,
	 50.0f,  50.0f,  50.0f,
	-50.0f,  50.0f,  50.0f,
	-50.0f,  50.0f, -50.0f,

	-50.0f, -50.0f, -50.0f,
	-50.0f, -50.0f,  50.0f,
	 50.0f, -50.0f, -50.0f,
	 50.0f, -50.0f, -50.0f,
	-50.0f, -50.0f,  50.0f,
	 50.0f, -50.0f,  50.0f
};

skybox::skybox()
{
}

//function to create VAO for skybox 
void skybox::genSkybox()
{
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glBindVertexArray(0);
}

//function to draw skybox 
void skybox::drawSkyBox(Shader skyboxShader, unsigned int skyboxTextureMap)
{
	skyboxShader.use();
	glBindVertexArray(skyboxVAO);
	glActiveTexture(GL_TEXTURE0 + skyboxTextureMap);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTextureMap);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	glDepthFunc(GL_LESS);

}