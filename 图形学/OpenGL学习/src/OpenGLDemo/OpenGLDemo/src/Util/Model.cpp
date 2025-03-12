#include "Model.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

Model::Model(const std::string& Path)
{
    Init(Path);
}

void Model::Init(const std::string& Path)
{
    directory = Path.substr(0, Path.find_last_of('/'));
    loadModel(Path);
}

void Model::Draw(Shader& shader)
{
    for (auto& meshItem : meshes)
    {
        meshItem.Draw(shader);
    }
}

void Model::loadModel(const std::string& Path)
{
    Assimp::Importer importer;
    const aiScene* scene = importer.ReadFile(Path, aiProcess_Triangulate | aiProcess_FlipUVs);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP::" << importer.GetErrorString() << std::endl;
        return;
    }

    processNode(scene->mRootNode, scene);
}

void Model::processNode(aiNode* node, const aiScene* scene)
{
    // 处理节点所有的网格（如果有的话）
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(std::move(processMesh(mesh, scene)));
    }
    // 接下来对它的子节点重复这一过程
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    std::vector<Vertex_Mesh> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture_Mesh> textures;

    //Position
    for (auto i = 0; i < mesh->mNumVertices; ++i)
    {
        Vertex_Mesh vertex;
        glm::vec3 vec(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.Position = vec;

        vec.x = mesh->mNormals[i].x;
        vec.y = mesh->mNormals[i].y;
        vec.z = mesh->mNormals[i].z;
        vertex.Normal = vec;

        if (mesh->mTextureCoords[0])
        {
            vertex.TexCoords = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
        }
        else
        {
            vertex.TexCoords = glm::vec2(0);
        }

        vertices.push_back(vertex);
    }

    //Indices
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }

    //Material
    if (mesh->mMaterialIndex >= 0)
    {
        auto material = scene->mMaterials[mesh->mMaterialIndex];
        std::vector<Texture_Mesh> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, ETextureType::E_Diffuse, scene);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        std::vector<Texture_Mesh> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, ETextureType::E_Specular, scene);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
    }
    return Mesh(vertices, indices, textures);
}

std::vector<Texture_Mesh> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, ETextureType tType, const aiScene* scene)
{
    std::vector<Texture_Mesh> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].fileName.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; 
                break;
            }
        }
        if (!skip)
        {
            GLuint textureId = GL_ZERO;
            std::string texturePath = directory + "/" + str.C_Str();
            auto innerTexture = scene->GetEmbeddedTexture(str.C_Str());
            if (innerTexture != nullptr)
            {
                textureId = cacheInnerTexture(innerTexture);
            }
            else {
                textureId = cacheTextures(texturePath);
            }

            auto aiTexture = scene->GetEmbeddedTexture(str.C_Str());
            Texture_Mesh texture;
            texture.tType = tType;
            texture.textureId = textureId;
            texture.path = texturePath;
            texture.fileName = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);
            std::cout << "add Texture " << (std::string)texture << std::endl;
        }
    }
    return textures;
}

GLuint Model::cacheTextures(const std::string& Path)
{
    GLuint Id = GL_ZERO;
    Texture t;
    t.InitWithModelTexture(Path);
    Id = t.GetTextureID();
    loadedTextures.push_back(std::move(t));
    return Id;
}

GLuint Model::cacheInnerTexture(const aiTexture* aiTex)
{
    GLuint Id = GL_ZERO;
    Texture t;
    t.InitWithModelInnterTexture(aiTex);
    Id = t.GetTextureID();
    loadedTextures.push_back(std::move(t));
    return Id;
}
