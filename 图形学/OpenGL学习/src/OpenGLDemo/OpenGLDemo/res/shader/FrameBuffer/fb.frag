#version 330 core

layout(location = 0) out vec4 o_color;

in vec2 v_TexCoord;

uniform sampler2D screenTexture;

void main() {
    o_color = vec4(v_TexCoord, 0.0f, 1.0f); 
}