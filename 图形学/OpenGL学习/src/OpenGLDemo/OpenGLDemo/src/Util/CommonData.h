#pragma once
#include "glm/glm.hpp"

// 顶点信息 v0 版本 后续根据需要可能新增 v1、v2 ...
struct Vertex_v0
{
	float position[3];			  // 顶点坐标
	float color[4];				  // 顶点颜色
	float texCoord[2];			  // 顶点的 UV 坐标	
	int texIndex;				  // 顶点使用贴图的序号
};


// 顶点信息 v1 版本 
struct Vertex_v1
{
	float position[3];			  // 顶点坐标
	float normal[3];			  // 法线贴图
};


// 材质
struct Material_v0
{
	glm::vec3 ambient;		// 环境光颜色
	glm::vec3 diffuse;		// 漫反射颜色 一般与环境光颜色相同
	glm::vec3 specular;		// 高光颜色

	int shininesss;			// 反光度
};

struct LightConfig_v0
{
	glm::vec3 ambient;		// 环境光颜色 使用 vec3 可以分别控制每个颜色的强度
	glm::vec3 diffuse;		// 漫反射光颜色 使用 vec3 可以分别控制每个颜色的强度
	glm::vec3 specular;		// 高光颜色 使用 vec3 可以分别控制每个颜色的强度

	glm::vec3 lightPos;		// 灯光的坐标
};
