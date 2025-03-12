#include "Texture.h"
#include "stb_image.h"

#include <assimp/scene.h>

Texture::~Texture()
{
	DeleteTexture();
}

Texture::Texture(Texture&& Other)
{
	m_LocalBuffer = Other.m_LocalBuffer;
	m_Width = Other.m_Width;
	m_Height = Other.m_Height;
	m_BPP = Other.m_BPP;
	m_FilePath = Other.m_FilePath;
	m_TextureId = Other.m_TextureId;
	m_LastBindSlot = Other.m_LastBindSlot;

	Other.m_LocalBuffer = nullptr;
	Other.m_FilePath.clear();
	Other.m_TextureId = GL_ZERO;
	Other.m_LastBindSlot = GL_ZERO;
}

void Texture::Init(const std::string& filePath)
{
	if (m_FilePath == filePath) {
		return;
	}

	DeleteTexture();
	m_FilePath = filePath;

	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	GL_CALL(glGenTextures(1, & m_TextureId));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, m_TextureId));

	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));		// 指定缩小器
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));		// 指定放大器
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));	// 设置贴图超过 0~1 之后的读取方式
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));	// 

	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));

	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));		// 解绑

	if (m_LocalBuffer) {
		stbi_image_free(m_LocalBuffer);
		m_LocalBuffer = nullptr;
	}
}

void Texture::InitWithModelTexture(const std::string& filePath)
{
	if (m_FilePath == filePath) {
		return;
	}

	DeleteTexture();
	m_FilePath = filePath;

	glGenTextures(1, &m_TextureId);

	int nrComponents;

	stbi_set_flip_vertically_on_load(0);
	unsigned char* data = stbi_load(filePath.c_str(), &m_Width, &m_Height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RED;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, m_TextureId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << filePath << std::endl;
		stbi_image_free(data);
	}
	data = nullptr;
}

void Texture::InitWithModelInnterTexture(const aiTexture* inTexture)
{
	glGenTextures(1, &m_TextureId);

	glBindTexture(GL_TEXTURE_2D, m_TextureId);
	glGenerateMipmap(GL_TEXTURE_2D);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	int nrChannels = 1;
	unsigned char* image_data = nullptr;
	if (inTexture->mHeight == 0)
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(inTexture->pcData), inTexture->mWidth, &m_Width, &m_Height, &nrChannels, 0);
	}
	else
	{
		image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(inTexture->pcData), inTexture->mWidth * inTexture->mHeight, &m_Width, &m_Height, &nrChannels, 0);
	}

	if (image_data != nullptr)
	{
		GLenum format = 1;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, format, m_Width, m_Height, 0, format, GL_UNSIGNED_BYTE, image_data);
	}
	image_data = nullptr;
}

void Texture::Bind(GLuint slot)
{
	m_LastBindSlot = slot;
	GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, m_TextureId));
}

void Texture::UnBind()
{
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}

int Texture::GetHeight()
{
	return m_Height;
}

int Texture::GetWidth()
{
	return m_Width;
}

GLuint Texture::GetBindSlot()
{
	return m_LastBindSlot;
}

GLuint Texture::GetTextureID()
{
	return m_TextureId;
}

void Texture::DeleteTexture()
{
	stbi_image_free(m_LocalBuffer);
	m_LocalBuffer = nullptr;

	GL_CALL(glDeleteTextures(1, &m_TextureId));
	m_TextureId = GL_ZERO;
}
