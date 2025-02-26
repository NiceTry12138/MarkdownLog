#version 450 core

layout(location = 0) out vec4 o_color;

in vec2 v_TexCoord;
in vec4 v_Color;
in float v_TexIndex;

void main() {
	o_color = v_Color;
}