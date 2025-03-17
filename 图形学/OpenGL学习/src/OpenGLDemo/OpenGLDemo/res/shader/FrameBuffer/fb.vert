#version 330 core

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

out vec2 v_TexCoord;

void main() {
	v_TexCoord = inTexCoord;
	gl_Position = vec4(inPosition, 1.0f);
}