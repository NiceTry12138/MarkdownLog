#pragma once
#include "CommonHead.h"

#define MAX_BONE_INFLUENCE 4

enum ETextureType
{
    E_NONE = 0,
    E_DIFFUSE = 1,
    E_SPECULAR = 2,
    E_AMBIENT = 3,
    E_EMISSIVE = 4,
    E_HEIGHT = 5,
    E_NORMALS = 6,
    E_SHININESS = 7,
    E_OPACITY = 8,
    E_DISPLACEMENT = 9,
    E_LIGHTMAP = 10,
    E_REFLECTION = 11,
    E_BASE_COLOR = 12,
    E_NORMAL_CAMERA = 13,
    E_EMISSION_COLOR = 14,
    E_METALNESS = 15,
    E_DIFFUSE_ROUGHNESS = 16,
    E_AMBIENT_OCCLUSION = 17,
    E_UNKNOWN = 18,
    E_SHEEN = 19,
    E_CLEARCOAT = 20,
    E_TRANSMISSION = 21,
    E_MAYA_BASE = 22,
    E_MAYA_SPECULAR = 23,
    E_MAYA_SPECULAR_COLOR = 24,
    E_MAYA_SPECULAR_ROUGHNESS = 25,
};

static std::map<ETextureType, std::string> G_TextureTypeToString = {
	{ ETextureType::E_NONE,							"NONE " },
	{ ETextureType::E_DIFFUSE,						"DIFFUSE " },
	{ ETextureType::E_SPECULAR,						"SPECULAR " },
	{ ETextureType::E_AMBIENT,						"AMBIENT " },
	{ ETextureType::E_EMISSIVE,						"EMISSIVE " },
	{ ETextureType::E_HEIGHT,						"HEIGHT " },
	{ ETextureType::E_NORMALS,						"NORMALS " },
	{ ETextureType::E_SHININESS,					"SHININESS " },
	{ ETextureType::E_OPACITY,						"OPACITY " },
	{ ETextureType::E_DISPLACEMENT,					"DISPLACEMENT " },
	{ ETextureType::E_LIGHTMAP,						"LIGHTMAP " },
	{ ETextureType::E_REFLECTION,					"REFLECTION " },
	{ ETextureType::E_BASE_COLOR,					"BASE_COLOR " },
	{ ETextureType::E_NORMAL_CAMERA,				"NORMAL_CAMERA " },
	{ ETextureType::E_EMISSION_COLOR,				"EMISSION_COLOR " },
	{ ETextureType::E_METALNESS,					"METALNESS " },
	{ ETextureType::E_DIFFUSE_ROUGHNESS,			"DIFFUSE_ROUGHNESS " },
	{ ETextureType::E_AMBIENT_OCCLUSION,			"AMBIENT_OCCLUSION " },
	{ ETextureType::E_UNKNOWN,						"UNKNOWN " },
	{ ETextureType::E_SHEEN,						"SHEEN " },
	{ ETextureType::E_CLEARCOAT,					"CLEARCOAT " },
	{ ETextureType::E_TRANSMISSION,					"TRANSMISSION " },
	{ ETextureType::E_MAYA_BASE,					"MAYA_BASE " },
	{ ETextureType::E_MAYA_SPECULAR,				"MAYA_SPECULAR " },
	{ ETextureType::E_MAYA_SPECULAR_COLOR,			"MAYA_SPECULAR_COLOR " },
	{ ETextureType::E_MAYA_SPECULAR_ROUGHNESS,		"MAYA_SPECULAR_ROUGHNESS " },
};

enum class ESkyBoxTextureType : uint8_t
{
	E_Right,			// 右
	E_Left,				// 左
	E_Top,				// 上
	E_Bottom,			// 下
	E_Front,			// 前
	E_Back,				// 后
};

static std::map<ESkyBoxTextureType, unsigned int> G_CubeTextureTypeMap = {
	{ ESkyBoxTextureType::E_Right	, GL_TEXTURE_CUBE_MAP_NEGATIVE_X  },
	{ ESkyBoxTextureType::E_Left	, GL_TEXTURE_CUBE_MAP_POSITIVE_X},
	{ ESkyBoxTextureType::E_Top		, GL_TEXTURE_CUBE_MAP_POSITIVE_Y },
	{ ESkyBoxTextureType::E_Bottom	, GL_TEXTURE_CUBE_MAP_NEGATIVE_Y },
	{ ESkyBoxTextureType::E_Front	, GL_TEXTURE_CUBE_MAP_NEGATIVE_Z},
	{ ESkyBoxTextureType::E_Back	, GL_TEXTURE_CUBE_MAP_POSITIVE_Z  },
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
		std::string typeStr = G_TextureTypeToString[tType];
		return "textureId = " + std::to_string(textureId) + " path = " + fileName + " type = " + typeStr;
	}
};

