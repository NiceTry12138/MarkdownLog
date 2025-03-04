#version 330 core

layout(location = 0) out vec4 o_color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
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

uniform vec3 viewPos;           // 相机坐标

void main() {
	// 环境光
	vec3 ambient = light.ambient * cubeMaterial.ambient;
	
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(light.lightPos - FragPos);

	float lightDis = distance(light.lightPos, FragPos);
	float disRate = 1 / lightDis / lightDis;

	// 漫反射
	float rate = max(dot(lightDir, normal), 0.0);	// 因为光线可能从物体的反面，此时忽略这个，所以用 max(0.0, )
	vec3 diffuse = disRate * cubeMaterial.diffuse * rate * light.diffuse;

	// 镜面反射
	vec3 enterViewDir = normalize(viewPos - FragPos);
		// 通过半程向量计算
	// vec3 halfV = (lightDir + enterViewDir) / length(lightDir + enterViewDir);
	// halfV = normalize(halfV);						// 半程向量
	// float specularRate = pow(max(dot(halfV, normal), 0.0), cubeMaterial.shininess);
		// 直接使用 OpenGL 的 reflect 计算
    vec3 reflectDir = reflect(-lightDir, normal);
    float specularRate = pow(max(dot(enterViewDir, reflectDir), 0.0), cubeMaterial.shininess);
	vec3 specular = disRate * cubeMaterial.specular * specularRate * light.specular;

	vec3 result = ambient + diffuse + specular;
	o_color = vec4(result, 1.0);
}