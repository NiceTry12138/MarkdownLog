#version 330 core

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoords;

uniform mat4 model;

layout (std140) uniform Matrices
{
    mat4 view;
    mat4 projection;
};

out VS_OUT {
    vec3 normal;
} vs_out;

void main() {
  	gl_Position = projection * view * model * vec4(inPosition, 1.0);
    mat3 normalMatrix = mat3(transpose(inverse(view * model)));
    vs_out.normal = normalize(vec3(vec4(normalMatrix * aNormal, 0.0)));
}
