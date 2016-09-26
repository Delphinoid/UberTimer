#include "sprite.hpp"
#include <math.h>
#include <SDL2/SDL.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

sprite::sprite(){
	position = glm::vec3(0.f);
	width = 1.f;
	height = 1.f;
	relPivot.x = 0.f;
	relPivot.y = 0.f;
	relPivot.z = 0.f;
	rotation = glm::vec3(0.f);
	scale = glm::vec3(1.f);
	translucency = 1.f;
	billboardX = false;
	billboardY = false;
	billboardZ = false;
	hudElement = false;
	sprTexture = NULL;
	currentAnim = 0;
	currentFrame = 0;
	frameProgress = 0.f;
}

bool sprite::sprLoad(std::string filePath, std::vector<textureWrapper> &allTextures){

	sprTexture = &allTextures.at(0);

	return true;

}

void sprite::sprAnimateTex(){

	if(sprTexture != NULL){
		if(frameProgress == 0){
			frameProgress = SDL_GetTicks();
		}
		sprTexture->texAnimate(1.f, currentAnim, currentFrame, frameProgress);
	}

}

void sprite::sprCreate(gfxProgram &gfxPrg){

	vertex3D tempVert;
	glm::vec3 scaledPivot = relPivot * scale;
	float scaledWidth  = width  * scale.x;
	float scaledHeight = height * scale.y;

	// Create the top left vertex
	tempVert.pos.x = position.x;
	tempVert.pos.y = position.y;
	tempVert.pos.z = 1.f;
	tempVert.u = 0.f;
	tempVert.v = 0.f;
	vertices[0] = tempVert;

	// Create the top right vertex
	tempVert.pos.x = position.x + scaledWidth;
	tempVert.pos.y = position.y;
	tempVert.pos.z = 1.f;
	tempVert.u = 1.f;
	tempVert.v = 0.f;
	vertices[1] = tempVert;

	// Create the bottom left vertex
	tempVert.pos.x = position.x;
	tempVert.pos.y = position.y + scaledHeight;
	tempVert.pos.z = 1.f;
	tempVert.u = 0.f;
	tempVert.v = 1.f;
	vertices[2] = tempVert;

	// Create the bottom right vertex
	tempVert.pos.x = position.x + scaledWidth;
	tempVert.pos.y = position.y + scaledHeight;
	tempVert.pos.z = 1.f;
	tempVert.u = 1.f;
	tempVert.v = 1.f;
	vertices[3] = tempVert;


	/** Not really needed here **/
	/*
	/* Billboarding *
	// Use the camera's X, Y and Z axes
	glm::vec3 axisX = glm::vec3(gfxPrg.viewMatrix[0][0], gfxPrg.viewMatrix[0][1], gfxPrg.viewMatrix[0][2]);
	glm::vec3 axisY = glm::vec3(gfxPrg.viewMatrix[1][0], gfxPrg.viewMatrix[1][1], gfxPrg.viewMatrix[1][2]);
	glm::vec3 axisZ = glm::vec3(gfxPrg.viewMatrix[2][0], gfxPrg.viewMatrix[2][1], gfxPrg.viewMatrix[2][2]);

	// Lock certain axes if needed
	if(!billboardX){
		axisX.y = 0.f;
		axisY.y = 1.f;
		axisZ.y = 0.f;
	}
	if(!billboardY){
		axisX.x = 1.f;
		axisY.x = 0.f;
		axisZ.x = 0.f;
	}
	if(!billboardZ){
		axisX.z = 0.f;
		axisY.z = 0.f;
		axisZ.z = 1.f;
	}

	// Generate a rotation matrix           X axis   Y axis   Z axis
	glm::mat4 billboardRotation = glm::mat4(axisX.x, axisY.x, axisZ.x, 0.f,
	                                        axisX.y, axisY.y, axisZ.y, 0.f,
	                                        axisX.z, axisY.z, axisZ.z, 0.f,
	                                        0.f,     0.f,     0.f,     1.f);


	/* Build the model transformation matrix *
	// Start with the identity matrix
	glm::mat4 modelViewMatrix = glm::translate(gfxPrg.viewMatrix, position);

	// Rotate the model
	//modelViewMatrix *= billboardRotation;  // Apply billboard rotation
	glm::vec3 rotationRadians = rotation;
	rotationRadians *= M_PI / 180;
	modelViewMatrix *= glm::mat4_cast(glm::quat(rotationRadians));

	// Translate the model by -scaledPivot to make it appear
	// to be rotating around scaledPivot
	modelViewMatrix = glm::translate(modelViewMatrix, -scaledPivot);


	/* Apply transformations to each vertex *
	glm::vec4 vertexPos;
	for(unsigned int d = 0; d < 4; d++){
		// We need to make the vertex positions a vec4 so we can multiply them by the 4x4 modelViewProjectionMatrix
		vertexPos = glm::vec4(vertices[d].pos, 1.f);
		vertexPos = modelViewMatrix * vertexPos;
		vertices[d].pos = glm::vec3(vertexPos);
	}
	*/

}

void sprite::sprOffsetTexture(float texFrag[4], float texWidth, float texHeight){

	// We can't pass unique textureFragment values for each individual sprite when batching. Therefore,
	// we have to do the offset calculations for each vertex UV here instead of in the shader
	for(unsigned int d = 0; d < 4; d++){
		vertices[d].u = ((vertices[d].u * texFrag[2]) + texFrag[0]) / texWidth;
		vertices[d].v = ((vertices[d].v * texFrag[3]) + texFrag[1]) / texHeight;
	}

}
