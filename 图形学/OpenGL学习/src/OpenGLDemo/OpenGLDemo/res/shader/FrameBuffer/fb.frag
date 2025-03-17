#version 330 core

layout(location = 0) out vec4 o_color;

in vec2 v_TexCoord;

uniform sampler2D screenTexture;
uniform float u_BlurSize;  // 模糊步长（通常为 1.0 / 纹理宽度 或 1.0 / 纹理高度）

void main() {
   // 高斯核权重（5次采样，权重和为1）
   float weights[5] = float[](
       0.227027,   // 中心点权重
       0.1945946,  // ±1 偏移
       0.1216216,  // ±2 偏移
       0.054054,   // ±3 偏移
       0.016216    // ±4 偏移
   );
   
    // 初始颜色（中心点）
   vec4 col = texture(screenTexture, v_TexCoord) * weights[0];
   
   // 横向模糊（左右各4次采样）
   for(int i = 1; i < 5; i++) {
       // 向右采样
       col += texture(screenTexture, v_TexCoord + vec2(u_BlurSize * i, 0.0)) * weights[i];
       // 向左采样 
       col += texture(screenTexture, v_TexCoord - vec2(u_BlurSize * i, 0.0)) * weights[i];
   }
   
   o_color = vec4(col.rgb, 1.0f);
   
}