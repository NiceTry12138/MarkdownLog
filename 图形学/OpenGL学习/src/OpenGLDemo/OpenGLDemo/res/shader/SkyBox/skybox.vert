#version 330 core

layout(location = 0) in vec3 inPosition;

out vec3 v_TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main() {
    v_TexCoord = inPosition;
    vec4 pos = projection * view * vec4(inPosition, 1.0);
    gl_Position = pos.xyww;
}