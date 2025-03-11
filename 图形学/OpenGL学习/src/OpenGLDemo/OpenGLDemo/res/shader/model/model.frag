#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D texture_diffuse1;

void main()
{    
    FragColor = texture(texture_diffuse1, TexCoords);
}

// #version 330 core
// 
// layout(location = 0) out vec4 o_color;
// 
// uniform sampler2D texture_diffuse1;
// uniform sampler2D texture_normal1;
// 
// uniform vec3 lightPos;
// 
// in vec3 FragPos;                // 顶点坐标
// in vec3 Normal;					// 法线向量
// in vec2 TexCoords;				// UV 坐标
// 
// uniform vec3 viewPos;           // 相机坐标
// 
// void main() {
//     // 环境光
//     vec3 ambient = texture(texture_diffuse1, TexCoords).rgb;
//     
//     // 漫反射 
//     // vec3 norm = normalize(Normal);
//     vec3 norm = texture(texture_normal1, TexCoords).rgb;
// 
//     vec3 lightDir = normalize(lightPos - FragPos);
//     float diff = max(dot(norm, lightDir), 0.0);
//     vec3 diffuse = diff * texture(texture_diffuse1, TexCoords).rgb;  
//     
//     // 镜面反射
//     vec3 viewDir = normalize(viewPos - FragPos);
//     vec3 reflectDir = reflect(-lightDir, norm);  
//     float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
//     vec3 specular = spec * texture(texture_diffuse1, TexCoords).rgb;  
//     
//     vec3 result = ambient + diffuse + specular;
//     o_color = vec4(result, 1.0);
// }