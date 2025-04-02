#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;
layout (location = 7) in mat4 instanceMatrix;

uniform mat4 model;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

out vec2 TexCoords;

uniform int UseInstanceMatrix;

void main() {
    TexCoords = aTexCoords;    

    if(UseInstanceMatrix < 1){
      	gl_Position = projection * view * model * vec4(inPosition, 1.0);
    } else {
      	gl_Position = projection * view * instanceMatrix * vec4(inPosition, 1.0);
    }
}
