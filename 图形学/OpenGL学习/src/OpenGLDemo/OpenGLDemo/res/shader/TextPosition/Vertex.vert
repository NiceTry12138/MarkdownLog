#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec4 inColor;
layout(location = 2) in vec2 inTexCoord;
layout(location = 3) in int inTexIndex;

out vec4 v_Color;
out vec2 v_TexCoord;
flat out int v_TexIndex;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
	gl_Position = projection * view * model * vec4(inPosition, 1.0);
	v_TexCoord = inTexCoord;
	v_TexIndex = inTexIndex;
	v_Color = inColor;
}