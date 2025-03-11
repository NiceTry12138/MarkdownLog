#pragma once
#include "CommonData.h"
#include "Shader.h"
#include "Texture.h"

class Mesh
{
public:
	std::vector<Vertex_Mesh> vertices;
	std::vector<GLuint> indices;
	std::vector<Texture_Mesh> textures;

public:
	Mesh() = default;
	Mesh(Mesh&& Other);
	Mesh(const std::vector<Vertex_Mesh>& inVertices, const std::vector<GLuint>& inIndices, const std::vector<Texture_Mesh>& inTextures);

	virtual ~Mesh() = default;

	void init(const std::vector<Vertex_Mesh>& inVertices, const std::vector<GLuint>& inIndices, const std::vector<Texture_Mesh>& inTextures);

	void Draw(Shader& shader);
protected:
	// 绑定数据信息
	void setUpMesh();
	void setUpTexture();

private:
	GLuint VAO{ GL_ZERO };		// vertex array object
	GLuint VBO{ GL_ZERO };		// vertex buffer
	GLuint EBO{ GL_ZERO };		// element buffer

	std::vector<Texture> loadedTextures;
};

