#version 330 core

layout(location = 0) out vec4 o_color;

in vec3 FragPos;                // 顶点坐标
in vec3 Normal;					// 法线向量

uniform vec3 viewPos;           // 相机坐标
uniform vec3 lightPos;          // 灯的坐标

uniform vec3 objectColor;       // 基本颜色
uniform vec3 lightColor;        // 灯光颜色

uniform float ambientStrength;  // 环境光强度
uniform float diffuseStrength;  // 漫反射强度
uniform float specularStrength;	// 镜面反射强度

void main() {
	// 环境光
	vec3 ambient = ambientStrength * lightColor;
	
	vec3 normal = normalize(Normal);
	vec3 lightDir = normalize(lightPos - FragPos);

	float lightDis = distance(lightPos, FragPos);
	float disRate = 1 / lightDis / lightDis;

	// 漫反射
	float rate = max(dot(lightDir, normal), 0.0);	// 因为光线可能从物体的反面，此时忽略这个，所以用 max(0.0, )
	vec3 diffuse = disRate * diffuseStrength * rate * lightColor;

	// 镜面反射
	vec3 enterViewDir = viewPos - FragPos;
	vec3 halfV = (lightDir + enterViewDir) / length(lightDir + enterViewDir);
	halfV = normalize(halfV);						// 半程向量
	float specularRate = pow(max(dot(halfV, normal), 0.0), 32);
	vec3 specular = disRate * specularStrength * specularRate * lightColor;

	vec3 result = (ambient + diffuse + specular) * objectColor;
	o_color = vec4(result, 1.0);
}