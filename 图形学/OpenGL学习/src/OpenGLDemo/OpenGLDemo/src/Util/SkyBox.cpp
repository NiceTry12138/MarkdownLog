#include "SkyBox.h"

SkyBox::SkyBox()
{
	m_TexturePath = {
		{ ESkyBoxTextureType::E_Right	, "res/textures/skybox/right.jpg"},
		{ ESkyBoxTextureType::E_Left	, "res/textures/skybox/left.jpg"},
		{ ESkyBoxTextureType::E_Top		, "res/textures/skybox/top.jpg"},
		{ ESkyBoxTextureType::E_Bottom	, "res/textures/skybox/bottom.jpg"},
		{ ESkyBoxTextureType::E_Front	, "res/textures/skybox/front.jpg"},
		{ ESkyBoxTextureType::E_Back	, "res/textures/skybox/back.jpg"},
	};
}

SkyBox::~SkyBox()
{
    glDeleteVertexArrays(1, &m_VAO);
}

void SkyBox::Init(const std::map<ESkyBoxTextureType, std::string> InTexturePath)
{
	m_TexturePath = InTexturePath;

	Init();
}

void SkyBox::Draw(const glm::mat4& view, const glm::mat4& projection)
{
    const int TextureSlot = 1;
    m_cubeImage.BindCubeTexture(TextureSlot);
    m_skyShader.Bind();
    m_skyShader.SetUniform1i("cubeTexture", TextureSlot);
    m_skyShader.SetUniformMat4f("projection", projection);
    m_skyShader.SetUniformMat4f("view", view);

    glBindVertexArray(m_VAO);
    glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
    glBindVertexArray(GL_ZERO);
}

void SkyBox::InitVAO()
{
    float skyboxVertices[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    glGenVertexArrays(1, &m_VAO);
    glBindVertexArray(m_VAO);

    GLuint VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // 解除绑定关系
    glBindVertexArray(GL_ZERO);
}

void SkyBox::InitImage()
{
    m_cubeImage.InitWithSkyBox(m_TexturePath);
}

void SkyBox::InitShader()
{
    m_skyShader.Init("res/shader/SkyBox/skybox.vert", "res/shader/SkyBox/skybox.frag");
}

void SkyBox::Init()
{
    InitImage();
    InitVAO();
    InitShader();
}

