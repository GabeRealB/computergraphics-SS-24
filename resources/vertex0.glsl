// Vertex Shader
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexColor;

uniform mat4 mvp_matrix; // Model-View-Projection matrix for our object

out vec3 fragmentColor;

void main(){
    gl_Position = mvp_matrix * vec4(vertexPosition_modelspace, 1.0);
    fragmentColor = vertexColor;
}