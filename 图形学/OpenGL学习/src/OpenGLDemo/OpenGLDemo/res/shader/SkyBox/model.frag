#version 330 core

layout(location = 0) out vec4 o_color;

uniform sampler2D texture_diffuse1;     // 漫反射贴图
uniform sampler2D texture_normal1;      // 法线贴图
uniform sampler2D texture_shiness1;     // 镜面反射贴图

in vec2 TexCoords;				// UV 坐标

void main() {
    o_color = vec4(texture(texture_diffuse1, TexCoords).rgb, 1.0);
}