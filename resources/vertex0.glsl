// Vertex Shader
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec2 vertexUV;

uniform mat4 mvp_matrix; // Model-View-Projection matrix for our object

out vec2 fragmentUV;

void main(){
    gl_Position = mvp_matrix * vec4(vertexPosition_modelspace, 1.0);
    fragmentUV = vertexUV;
}