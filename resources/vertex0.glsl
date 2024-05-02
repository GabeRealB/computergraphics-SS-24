// Vertex Shader
#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
uniform mat4 model_matrix;

out vec4 fragmentColor; // Output color to the fragment shader

void main(){
    gl_Position = model_matrix * vec4(vertexPosition_modelspace, 1.0);
    fragmentColor = vec4(0.8, 0.8, 0.8, 1); // Pass color to the fragment shader
}