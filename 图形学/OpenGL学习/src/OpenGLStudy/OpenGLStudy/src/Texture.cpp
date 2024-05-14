#include "Texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include "vendor/stb_image/stb_image.h"

Texture::Texture(const std::string& filePath)
{
	m_FilePath = filePath;
	
	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(filePath.c_str(), &m_Width, &m_Height, &m_BPP, 4);

	GL_CALL(glGenTextures(1, &m_RenderID));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RenderID));

	// һ��Ҫ���õ����� ����ֻ�ܵõ���ɫ����
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));	// ָ����С��
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));	// ָ���Ŵ���
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	// �������� GL_RGBA8 �����8 �����ʾÿ��ͨ��վ����λ
	GL_CALL(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Width, m_Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer));

	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));

	if (m_LocalBuffer) {
		stbi_image_free(m_LocalBuffer);
		m_LocalBuffer = nullptr;
	}
}

Texture::~Texture()
{
	if (m_LocalBuffer != nullptr) {
		stbi_image_free(m_LocalBuffer);
	}

	glDeleteTextures(1, &m_RenderID);
}

void Texture::Bind(unsigned int slot) const
{
	// ������ ����ͼ�󶨵���Ӧ�����
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RenderID);
}

void Texture::Unbind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}
