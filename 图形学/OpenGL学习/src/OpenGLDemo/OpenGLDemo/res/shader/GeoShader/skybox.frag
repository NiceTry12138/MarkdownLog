#version 330 core

layout(location = 0) out vec4 o_color;

in vec3 v_TexCoord;

uniform samplerCube cubeTexture;

void main() {
    o_color = texture(cubeTexture, v_TexCoord);
}