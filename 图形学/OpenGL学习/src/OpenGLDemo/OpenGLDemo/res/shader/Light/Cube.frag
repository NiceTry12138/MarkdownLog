#version 330 core

layout(location = 0) out vec4 o_color;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    int shininess;
}; 

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

	vec3 lightPos;
}; 

uniform Material cubeMaterial;
uniform Light light;

in vec3 FragPos;                // 顶点坐标
in vec3 Normal;					// 法线向量
in vec2 TexCoords;				// UV 坐标

uniform vec3 viewPos;           // 相机坐标

void main() {
    // 环境光
    vec3 ambient = light.ambient * texture(cubeMaterial.diffuse, TexCoords).rgb;
    
    // 漫反射 
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(cubeMaterial.diffuse, TexCoords).rgb;  
    
    // 镜面反射
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), cubeMaterial.shininess);
    vec3 specular = light.specular * spec * texture(cubeMaterial.specular, TexCoords).rgb;  
    
    vec3 result = ambient + diffuse + specular;
    o_color = vec4(result, 1.0);
}