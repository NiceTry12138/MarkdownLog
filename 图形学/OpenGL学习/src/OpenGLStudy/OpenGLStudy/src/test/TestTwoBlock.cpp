#include "TestTwoBlock.h"

TestModule::TestTwoBlock::~TestTwoBlock()
{
	if (m_va != nullptr) delete m_va;
	if (m_ibo != nullptr) delete m_ibo;
	if (m_vb != nullptr) delete m_vb;
	if (m_shader != nullptr) delete m_shader;
	if (m_texture != nullptr) delete m_texture;

	m_texture = nullptr;
	m_va = nullptr;
	m_ibo = nullptr;
	m_vb = nullptr;
	m_shader = nullptr;
}

void TestModule::TestTwoBlock::Init()
{
	Test::Init();

	float positions[] = {
		-50, -50, 0.0f, 0.0f,
		 50, -50, 1.0f, 0.0f,
		 50,  50, 1.0f, 1.0f,
		-50,  50, 0.0f, 1.0f,

		150, 150, 0.0f, 0.0f,
		250, 150, 1.0f, 0.0f,
		250, 250, 1.0f, 1.0f,
		150, 250, 0.0f, 1.0f,
	};

	GLuint indeices[] = {
		0, 1, 2,
		2, 3, 0,

		4, 5, 6,
		6, 7, 4,
	};

	// 顶点数组	
	m_va = new VertexArray();

	// 顶点缓冲 
	m_vb = new VertexBuffer(positions, sizeof(float) * 4 * 8);

	// 布局信息
	VertexBufferLayout layout;
	layout.Push<float>(2);	// 前两个是 顶点
	layout.Push<float>(2);	// 后两个是 UV 坐标

	m_va->AddBuffer(*m_vb, layout);

	// 索引缓冲
	m_ibo = new IndexBuffer(indeices, 12);

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

void TestModule::TestTwoBlock::Exit()
{
}

void TestModule::TestTwoBlock::OnUpdate(float deltaTIme)
{
}

void TestModule::TestTwoBlock::OnRender()
{
	static glm::mat4 mvp = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

	m_render.Clear();

	glm::mat4 translationA = glm::translate(glm::mat4(1.0f), m_CameraTransition);
	m_shader->SetUniformMat4f("u_MVP", mvp * translationA);
	m_render.Draw(*m_va, *m_ibo, *m_shader);
}

void TestModule::TestTwoBlock::OnImGuiRender()
{
	ImGui::Begin("Update Transition");
	ImGui::SliderFloat3("Transition A", &m_CameraTransition.x, 0.0f, 500.0f);
	ImGui::End();
}
