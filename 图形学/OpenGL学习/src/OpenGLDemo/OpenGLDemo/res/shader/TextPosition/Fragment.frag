#version 330 core

layout(location = 0) out vec4 o_color;

in vec4 v_Color;
in vec2 v_TexCoord;
flat in int v_TexIndex;

uniform sampler2D u_Texture0;
uniform sampler2D u_Texture1;

void main() {
    vec4 texColor = v_TexIndex == 0 ? 
        texture(u_Texture0, v_TexCoord) : 
        texture(u_Texture1, v_TexCoord);
    
    o_color = mix(texColor, v_Color, 0.3); // 混合纹理和颜色
}