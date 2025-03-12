#pragma once
#include "CommonHead.h"

#define MAX_BONE_INFLUENCE 4

enum class ETextureType : GLuint
{
	E_Diffuse = 0,		// 漫反射
	E_Specular = 1,		// 镜面反射
	E_Normal = 2,		// 法线
	E_Height = 3,		// 高度
};

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

// 顶点信息 v2 版本 
struct Vertex_v2
{
	float position[3];			  // 顶点坐标
	float normal[3];			  // 法线贴图
	float texCoords[2];			  // UV 坐标
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


struct Material_v1
{
	GLuint diffuse;			// 漫反射光贴图
	GLuint specular;		// 高光颜色贴图

	int shininesss;			// 反光度
};

struct Vertex_Mesh {
	glm::vec3 Position;							// 顶点坐标
	glm::vec3 Normal;							// 顶点法线
	glm::vec2 TexCoords;						// UV 坐标
	glm::vec3 Tangent;							// 切线向量
	glm::vec3 Bitangent;						// 副切线向量
	int m_BoneIDs[MAX_BONE_INFLUENCE];			// 影响该顶点的骨骼 ID
	float m_Weights[MAX_BONE_INFLUENCE];		// 每个骨骼对该顶点的影响权重
};

struct Texture_Mesh {
	GLuint textureId;
	ETextureType tType;
	std::string path;							// 全路径名称
	std::string fileName;						// 文件名称

	operator std::string() {
		std::string typeStr = "unknow type";
		switch (tType)
		{
		case ETextureType::E_Diffuse:
			typeStr = "diffuse";
			break;
		case ETextureType::E_Specular:
			typeStr = "specular";
			break;
		case ETextureType::E_Normal:
			typeStr = "normal";
			break;
		case ETextureType::E_Height:
			typeStr = "height";
			break;
		}

		return "textureId = " + std::to_string(textureId) + " path = " + path + " type = " + typeStr;
	}
};

