#version 330 core

layout(location = 0) out vec4 o_color;

uniform vec4 baseColor;

void main() {
    o_color = vec4(1.0f, 1.0f, 1.0f, 1.0f); // 混合纹理和颜色
}