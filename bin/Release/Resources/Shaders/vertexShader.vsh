#version 450 core

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 vertexUV;
layout(location = 2) in vec3 vertexNormals;
uniform mat4 modelViewProjectionMatrix;
uniform sampler2D textureSampler;
out vec2 UV;

void main(){
	gl_Position = modelViewProjectionMatrix * vec4(vertexPosition, 1.0f);
	UV.x = vertexUV.x;
	UV.y = vertexUV.y;
}