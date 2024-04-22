// Vertex Shader
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
uniform mat4 MVP; // Model-View-Projection matrix for our object

void main(){
    gl_Position = MVP * vec4(vertexPosition_modelspace, 1.0);
}