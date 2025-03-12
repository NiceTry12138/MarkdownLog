#pragma once
#include "CommonData.h"
#include "Mesh.h"
#include "Shader.h"

struct aiNode;
struct aiMesh;
struct aiScene;
struct aiMaterial;
struct aiTexture;
enum aiTextureType;

class Model
{
public:
	Model(const std::string& Path);
	Model() = default;

	void Init(const std::string& Path);
	void Draw(Shader& shader);
protected:
	void loadModel(const std::string& Path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<Texture_Mesh> loadMaterialTextures(aiMaterial* mat, aiTextureType type, ETextureType tType, const aiScene* scene);

	GLuint cacheTextures(const std::string& Path);
	GLuint cacheInnerTexture(const aiTexture* aiTex);		// 处理内置材质 

private:
	std::vector<Texture_Mesh> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
	std::vector<Mesh> meshes;
	std::string directory;

	std::vector<Texture> loadedTextures = std::vector<Texture>(128);
};