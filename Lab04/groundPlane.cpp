#include "GroundPlane.h"

GroundPlane::GroundPlane() {

}
GroundPlane::GroundPlane(vec3 initalLocation) {
	groundLocation = initalLocation;
}


//function to create VAO for ground plane using mesh 
void GroundPlane::genGround() {
	Mesh mesh;
	groundMesh = mesh.load_mesh("mesh/Rockwall.obj");
	//positions
	glGenBuffers(1, &vpvbo);
	glBindBuffer(GL_ARRAY_BUFFER, vpvbo);
	glBufferData(GL_ARRAY_BUFFER, groundMesh.mPointCount * sizeof(vec3), &groundMesh.mVertices[0], GL_STATIC_DRAW);
	//normals
	glGenBuffers(1, &vnvbo);
	glBindBuffer(GL_ARRAY_BUFFER, vnvbo);
	glBufferData(GL_ARRAY_BUFFER, groundMesh.mPointCount * sizeof(vec3), &groundMesh.mNormals[0], GL_STATIC_DRAW);
	//texture
	glGenBuffers(1, &vtvbo);
	glBindBuffer(GL_ARRAY_BUFFER, vtvbo);
	glBufferData(GL_ARRAY_BUFFER, groundMesh.mPointCount * sizeof(vec2), &groundMesh.mTextureCoords[0], GL_STATIC_DRAW);

	glBindVertexArray(0); //break existing binding
}
//function to draw ground plane scaled and translated 
void GroundPlane::drawGround(Shader shader, int width, int height, unsigned int diffMap) {

	shader.use();
	//locations in shader
	int matrix_location = glGetUniformLocation(shader.ID, "model");
	GLuint loc1 = 0;
	GLuint loc2 = 1;
	GLuint loc3 = 2;

	//form model 
	mat4 model = identity_mat4();
	model = scale(model, vec3(500.0f, 500.0f, 500.0f)); //scale to fill screen 
	model = translate(model, groundLocation); //translate to location
	shader.setMat4("model", model);

	//texture mapping 
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, diffMap);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, diffMap);


	glEnableVertexAttribArray(loc1);
	glBindBuffer(GL_ARRAY_BUFFER, vpvbo);
	glVertexAttribPointer(loc1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc2);
	glBindBuffer(GL_ARRAY_BUFFER, vnvbo);
	glVertexAttribPointer(loc2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(loc3);
	glBindBuffer(GL_ARRAY_BUFFER, vtvbo);
	glVertexAttribPointer(loc3, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	// draw plane 
	glUniformMatrix4fv(matrix_location, 1, GL_FALSE, model.m);
	glDrawArrays(GL_TRIANGLES, 0, groundMesh.mPointCount);
}