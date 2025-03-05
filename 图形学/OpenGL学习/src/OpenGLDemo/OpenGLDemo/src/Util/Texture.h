#pragma once
#include "CommonHead.h"

class Texture
{
public:
	virtual ~Texture();

	void Init(const std::string& filePath);

	void Bind(GLuint slot = 0);
	void UnBind();

	int GetHeight();
	int GetWidth();

	GLuint GetBindSlot();

	void DeleteTexture();
private:
	GLubyte* m_LocalBuffer{ nullptr };
	// BPP Bit Per Pixel 表示每个像素的位数， 24bit-color 通常由三通道颜色组成；32bit-color 通常由四通道颜色组成
	int m_Width = 0, m_Height = 0, m_BPP = 0;
	std::string m_FilePath;

	GLuint m_TextureId{ GL_ZERO };
	GLuint m_LastBindSlot;
};

