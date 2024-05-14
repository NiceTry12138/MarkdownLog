#pragma once
#include "Util.h"

class Texture
{
private:
	GLuint m_RenderID = 0;
	std::string m_FilePath;
	GLubyte* m_LocalBuffer{ nullptr };
	// BPP Bit Per Pixel ��ʾÿ�����ص�λ���� 24bit-color ͨ������ͨ����ɫ��ɣ�32bit-color ͨ������ͨ����ɫ���
	int m_Width = 0, m_Height = 0, m_BPP = 0;

public:
	Texture(const std::string& filePath);
	~Texture();
	
	// slot ����󶨲��
	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
};

