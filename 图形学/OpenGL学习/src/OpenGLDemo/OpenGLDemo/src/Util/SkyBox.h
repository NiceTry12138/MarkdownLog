#pragma once
#include "CommonData.h"

#include "Texture.h"
#include "Shader.h"


class SkyBox
{
public:
	SkyBox();
	~SkyBox();

	void Init();
	void Init(const std::map<ESkyBoxTextureType, std::string> InTexturePath);

	void Draw(const glm::mat4& view, const glm::mat4& projection);

protected:
	void InitVAO();
	void InitImage();
	void InitShader();

private:
	Texture m_cubeImage;
	Shader m_skyShader;

	GLuint m_VAO;

	std::map<ESkyBoxTextureType, std::string> m_TexturePath;
};

