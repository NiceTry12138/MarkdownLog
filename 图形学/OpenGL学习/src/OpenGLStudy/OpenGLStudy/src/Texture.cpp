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

	// 一定要设置的纹理 否则只能得到黑色纹理
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));	// 指定缩小器
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));	// 指定放大器
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GL_CALL(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));

	// 传递数据 GL_RGBA8 后面加8 用与表示每个通道站多少位
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

	GL_CALL(glDeleteTextures(1, &m_RenderID));
}

void Texture::Bind(unsigned int slot) const
{
	// 激活插槽 将贴图绑定到对应插槽中
	GL_CALL(glActiveTexture(GL_TEXTURE0 + slot));
	GL_CALL(glBindTexture(GL_TEXTURE_2D, m_RenderID));
}

void Texture::Unbind() const
{
	GL_CALL(glBindTexture(GL_TEXTURE_2D, 0));
}
