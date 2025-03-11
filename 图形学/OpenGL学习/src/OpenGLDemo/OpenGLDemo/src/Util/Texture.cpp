#include "Texture.h"
#include "stb_image.h"

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
	Other.m_FilePath.empty();
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

void Texture::DeleteTexture()
{
	stbi_image_free(m_LocalBuffer);
	m_LocalBuffer = nullptr;

	GL_CALL(glDeleteTextures(1, &m_TextureId));
	m_TextureId = GL_ZERO;
}
