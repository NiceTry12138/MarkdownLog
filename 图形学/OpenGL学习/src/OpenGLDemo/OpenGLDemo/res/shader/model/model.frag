#version 330 core

layout(location = 0) out vec4 o_color;

uniform sampler2D texture_diffuse1;     // 漫反射贴图
uniform sampler2D texture_normal1;      // 法线贴图
uniform sampler2D texture_shiness1;     // 镜面反射贴图

uniform vec3 lightPos;          // 灯光坐标
uniform int shineness;

in vec3 FragPos;                // 顶点坐标
in vec3 Normal;					// 法线向量
in vec2 TexCoords;				// UV 坐标

uniform vec3 viewPos;           // 相机坐标

void main() {
    // 漫反射 
    vec3 norm = texture(texture_normal1, TexCoords).rgb;

    float dis = distance(lightPos, FragPos);
    float rate = 1.0f / dis;

    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * texture(texture_diffuse1, TexCoords).rgb * rate;  
    
    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0f), shineness);
    vec3 specular = spec * texture(texture_shiness1, TexCoords).rgb * 0.01f * 1 / rate;  
    
    vec3 result = diffuse + specular;
    o_color = vec4(result, 1.0);
}