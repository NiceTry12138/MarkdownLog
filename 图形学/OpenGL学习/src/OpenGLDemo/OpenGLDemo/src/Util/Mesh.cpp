#include "Mesh.h"

Mesh::Mesh(Mesh&& Other)
{
	VAO = Other.VAO;
	VBO = Other.VBO;
	EBO = Other.EBO;

	vertices = std::move(Other.vertices);
	indices = std::move(Other.indices);
	textures = std::move(Other.textures);

	Other.VAO = GL_ZERO;
	Other.VBO = GL_ZERO;
	Other.EBO = GL_ZERO;
}

Mesh::Mesh(const std::vector<Vertex_Mesh>& inVertices, const std::vector<GLuint>& inIndices, const std::vector<Texture_Mesh>& inTextures)
{
	this->init(inVertices, inIndices, inTextures);
}

void Mesh::init(const std::vector<Vertex_Mesh>& inVertices, const std::vector<GLuint>& inIndices, const std::vector<Texture_Mesh>& inTextures)
{
	this->vertices = inVertices;
	this->indices = inIndices;
	this->textures = inTextures;

	setUpMesh();
}

void Mesh::Draw(Shader& shader, int count)
{
	shader.Bind();

	GLuint textureDiffuseIndex = 1;
	GLuint textureSpecularIndex = 1;
	GLuint textureHeightIndex = 1;
	GLuint textureNormalIndex = 1;
	
	// shader 中 slot 名称的定义大概如下
	// uniform sampler2D texture_diffuse1;
	// uniform sampler2D texture_diffuse2;
	// uniform sampler2D texture_diffuse3;
	// uniform sampler2D texture_specular1;
	// uniform sampler2D texture_specular2;

	for (GLuint index = 0; index < textures.size(); ++index) {
		glActiveTexture(GL_TEXTURE0 + index);
		glBindTexture(GL_TEXTURE_2D, textures[index].textureId);

		std::string slotName;
		switch (textures[index].tType)
		{
		case ETextureType::E_DIFFUSE:
			slotName = "texture_diffuse" + std::to_string(textureDiffuseIndex++);
			break;
		case ETextureType::E_SPECULAR:
			slotName = "texture_specular" + std::to_string(textureSpecularIndex++);
			break;
		case ETextureType::E_HEIGHT:
			slotName = "texture_normal" + std::to_string(textureHeightIndex++);
			break;
		case ETextureType::E_NORMALS:
			slotName = "texture_normal" + std::to_string(textureNormalIndex++);
			break;
		case ETextureType::E_SHININESS:
			slotName = "texture_shiness" + std::to_string(textureNormalIndex++);
			break;
		}
		shader.SetUniform1i(slotName, index);
		//std::cout << "use texture: id = " << (std::string)textures[index] << " slotId = " << index << " slotName = " << slotName << std::endl;
	}

	//std::cout << std::endl  << std::endl;

	// draw mesh
	glBindVertexArray(VAO);

	if (count != 0)
	{
		glDrawElementsInstanced(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0, count);
	}
	else {
		glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(indices.size()), GL_UNSIGNED_INT, 0);
	}

	// 清空 贴图 和 VAO 的绑定
	glBindVertexArray(GL_ZERO);
	glActiveTexture(GL_TEXTURE0);
}

void Mesh::AddInstanceData()
{
	glBindVertexArray(VAO);

	// 这里可以整合成一个 for 循环 
	int Index = 7;
	glEnableVertexAttribArray(Index);
	glVertexAttribPointer(Index, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glVertexAttribDivisor(Index, 1);

	++Index;
	glEnableVertexAttribArray(Index);
	glVertexAttribPointer(Index, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glVertexAttribDivisor(Index, 1);
	
	++Index;
	glEnableVertexAttribArray(Index);
	glVertexAttribPointer(Index, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 2));
	glVertexAttribDivisor(Index, 1);

	++Index;
	glEnableVertexAttribArray(Index);
	glVertexAttribPointer(Index, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4) * 3));
	glVertexAttribDivisor(Index, 1);

	glBindVertexArray(GL_ZERO);
}

GLuint Mesh::GetVAO() const
{
	return VAO;
}

void Mesh::setUpMesh()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	// 创建并绑定 ArrayBuffer
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex_Mesh), vertices.data(), GL_STATIC_DRAW);

	// 创建并绑定 顶点信息
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, sizeof(GL_FLOAT), GL_FALSE, sizeof(Vertex_Mesh), (void*)offsetof(Vertex_Mesh, Position));

	std::vector<GLuint> attribSize{ 3, 3, 2, 3, 3, MAX_BONE_INFLUENCE, MAX_BONE_INFLUENCE };
	std::vector<GLuint> attribType{ GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_FLOAT, GL_INT, GL_FLOAT };
	std::vector<GLuint> attribOffset{ 
		offsetof(Vertex_Mesh, Position), 
		offsetof(Vertex_Mesh, Normal), 
		offsetof(Vertex_Mesh, TexCoords), 
		offsetof(Vertex_Mesh, Tangent), 
		offsetof(Vertex_Mesh, Bitangent), 
		offsetof(Vertex_Mesh, m_BoneIDs), 
		offsetof(Vertex_Mesh, m_Weights) 
	};

	for (int index = 0; index < attribSize.size(); ++index)
	{
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, attribSize[index], attribType[index], GL_FALSE, sizeof(Vertex_Mesh), (void*)attribOffset[index]);
	}
	
	// 解绑 避免影响其他流程 或者被其他流程影响
	glBindVertexArray(GL_ZERO);
}