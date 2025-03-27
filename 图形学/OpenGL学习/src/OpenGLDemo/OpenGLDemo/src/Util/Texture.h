#pragma once
//#include "CommonHead.h"
#include "CommonData.h"

class aiTexture;

class Texture
{
public:
	virtual ~Texture();
	Texture(Texture&& Other);
	Texture() = default;
	Texture(const Texture& Other) = default;

	void Init(const std::string& filePath);
	void InitWithModelTexture(const std::string& filePath);
	void InitWithModelInnterTexture(const aiTexture* inTexture);
	void InitWithSkyBox(const std::map<ESkyBoxTextureType, std::string>& InTexturesPath);

	void BindCubeTexture(GLuint slot = 0);
	void Bind(GLuint slot = 0);
	void UnBind();

	int GetHeight();
	int GetWidth();

	GLuint GetBindSlot();
	GLuint GetTextureID();

	void DeleteTexture();
private:
	GLubyte* m_LocalBuffer{ nullptr };
	// BPP Bit Per Pixel ��ʾÿ�����ص�λ���� 24bit-color ͨ������ͨ����ɫ��ɣ�32bit-color ͨ������ͨ����ɫ���
	int m_Width = 0, m_Height = 0, m_BPP = 0;
	std::string m_FilePath;

	GLuint m_TextureId{ GL_ZERO };
	GLuint m_LastBindSlot;
};

