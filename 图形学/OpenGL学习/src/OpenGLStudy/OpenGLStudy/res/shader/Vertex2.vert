#version 450 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 texCoord;
layout(location = 3) in float texIndex;

uniform mat4 u_MVP;

out vec2 v_TexCoord;
out vec4 v_Color;
out float v_TexIndex;

void main() {
	gl_Position = u_MVP * vec4(position, 1.0);
	v_TexCoord = texCoord;
	v_TexIndex = int(texIndex);
	v_Color = inColor;
}