#pragma once
#include "Util.h"

class Texture
{
private:
	GLuint m_RenderID = 0;
	std::string m_FilePath;
	GLubyte* m_LocalBuffer{ nullptr };
	// BPP Bit Per Pixel 表示每个像素的位数， 24bit-color 通常由三通道颜色组成；32bit-color 通常由四通道颜色组成
	int m_Width = 0, m_Height = 0, m_BPP = 0;

public:
	Texture(const std::string& filePath);
	~Texture();
	
	// slot 用与绑定插槽
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};

