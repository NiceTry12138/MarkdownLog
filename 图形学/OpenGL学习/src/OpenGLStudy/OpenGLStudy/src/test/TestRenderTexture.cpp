#include "TestRenderTexture.h"
#include "xutility"

TestModule::TestRenderTexture::~TestRenderTexture()
{
	if(m_va != nullptr) delete m_va;
	if(m_ibo != nullptr) delete m_ibo;
	if(m_vb != nullptr) delete m_vb;
	if(m_shader != nullptr) delete m_shader;
	if(m_texture != nullptr) delete m_texture;

	m_texture = nullptr;
	m_va = nullptr;
	m_ibo = nullptr;
	m_vb = nullptr;
	m_shader = nullptr;
}

void TestModule::TestRenderTexture::Init()
{
    Test::Init();

	float positions[] = {
		-50, -50, 0.0f, 0.0f,
		50, -50, 1.0f, 0.0f,
		50,  50, 1.0f, 1.0f,
		-50,  50, 0.0f, 1.0f,
	};

	GLuint indeices[] = {
		0, 1, 2,
		2, 3, 0
	};

	// 顶点数组	
	m_va = new VertexArray();

	// 顶点缓冲 
	m_vb = new VertexBuffer(positions, sizeof(float) * 4 * 4);

	// 布局信息
	VertexBufferLayout layout;
	layout.Push<float>(2);	// 前两个是 顶点
	layout.Push<float>(2);	// 后两个是 UV 坐标

	m_va->AddBuffer(*m_vb, layout);

	// 索引缓冲
	m_ibo = new IndexBuffer(indeices, 6);

	m_texture = new Texture("res/textures/ChernoLogo.png");
	m_texture->Bind(0);

	m_shader = new Shader("res/shader/Vertex.vert", "res/shader/Fragment.frag");
	m_shader->Bind();
	m_shader->SetUniform1i("u_Texture", 0);

	// 清除所有绑定关系
	m_va->Unbind();
	m_vb->UnBind();
	m_ibo->Unbind();
	m_shader->Unbind();
}

void TestModule::TestRenderTexture::Exit()
{
}

void TestModule::TestRenderTexture::OnUpdate(float deltaTime)
{
}

void TestModule::TestRenderTexture::OnRender()
{
	static glm::mat4 mvp = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

	m_render.Clear();

	glm::mat4 translationA = glm::translate(glm::mat4(1.0f), m_TransitaionA);
	m_shader->SetUniformMat4f("u_MVP", mvp * translationA);
	m_render.Draw(*m_va, *m_ibo, *m_shader);

	// 绘制第二张图片
	glm::mat4 translationB = glm::translate(glm::mat4(1.0f), m_TransitaionB);
	m_shader->SetUniformMat4f("u_MVP", mvp * translationB);
	m_render.Draw(*m_va, *m_ibo, *m_shader);

}

void TestModule::TestRenderTexture::OnImGuiRender()
{
    ImGui::Begin("Update Transition");
	ImGui::SliderFloat3("Transition A", &m_TransitaionA.x, 0.0f, 500.0f);
	ImGui::SliderFloat3("Transition B", &m_TransitaionB.x, 0.0f, 500.0f);
	ImGui::End();
}
