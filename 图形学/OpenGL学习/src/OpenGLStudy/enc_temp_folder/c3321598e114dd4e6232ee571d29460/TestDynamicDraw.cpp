#include "TestDynamicDraw.h"

void TestModule::TestDynamicDraw::Init()
{
	// ��ʼ��������Ϣ ���ꡢ  ��ɫ��  TexCoord��  ���
	m_OriginVertexs[0] = { -1.5f, -0.5f, 0.0f,    0.1f, 0.6f, 0.9f, 1.0f,    0.0f, 0.0f,    0.0f };
	m_OriginVertexs[1] = { -0.5f, -0.5f, 0.0f,    0.1f, 0.6f, 0.9f, 1.0f,    1.0f, 0.0f,    0.0f };
	m_OriginVertexs[2] = { -0.5f,  0.5f, 0.0f,    0.1f, 0.6f, 0.9f, 1.0f,    1.0f, 1.0f,    0.0f };
	m_OriginVertexs[3] = { -1.5f,  0.5f, 0.0f,    0.1f, 0.6f, 0.9f, 1.0f,    0.0f, 1.0f,    0.0f };
	m_OriginVertexs[4] = {  0.5f, -0.5f, 0.0f,    0.9f, 0.6f, 0.1f, 1.0f,    0.0f, 0.0f,    1.0f };
	m_OriginVertexs[5] = {  1.5f, -0.5f, 0.0f,    0.9f, 0.6f, 0.1f, 1.0f,    1.0f, 0.0f,    1.0f };
	m_OriginVertexs[6] = {  1.5f,  0.5f, 0.0f,    0.9f, 0.6f, 0.1f, 1.0f,    1.0f, 1.0f,    1.0f };
	m_OriginVertexs[7] = {  0.5f,  0.5f, 0.0f,    0.9f, 0.6f, 0.1f, 1.0f,    0.0f, 1.0f,    1.0f };

	// �������� VAO
	glCreateVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// �������� VBO
	glCreateBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 1000, nullptr, GL_DYNAMIC_DRAW);	// Ԥ������� 1000 �� Vertex ���ڴ��ַ ��Ȼ�����ò���

	// ���� VAO �ڴ�ṹ
	GL_CALL(glEnableVertexArrayAttrib(m_VAO, 0));
	// ������ ���Ϊ 0�� ������ 3 �� GL_FLOAT, ����Ҫ��һ����������С�� sizeof(Vertex) λ��ƫ���� offsetof(Vertex, Position)
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position)));

	GL_CALL(glEnableVertexArrayAttrib(m_VAO, 1));
	GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color)));

	GL_CALL(glEnableVertexArrayAttrib(m_VAO, 2));
	GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoord)));

	GL_CALL(glEnableVertexArrayAttrib(m_VAO, 3));
	GL_CALL(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TextureIndex)));

	// ������������ 
	GLuint indices[] = {
		0, 1, 2, 2, 3, 0,
		4, 5, 6, 6, 7, 4,
	};

	GL_CALL(glCreateBuffers(1, &m_IB));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	m_T1.Init("res/textures/ChernoLogo.png");
	m_T2.Init("res/textures/HazelLogo.png");

	m_Shader.Init("res/shader/Vertex2.vert", "res/shader/Fragment2.frag");

	GL_CALL(glBindVertexArray(0));
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, 0));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));

	m_T1.Unbind();
	m_T2.Unbind();
	m_Shader.Unbind();
}

void TestModule::TestDynamicDraw::Exit()
{

}

void TestModule::TestDynamicDraw::OnUpdate(float deltaTime)
{
	const int BlueCount = 4;
	const int RedCount = 4;
	for (int i = 0; i < BlueCount; i++)
	{
		m_Vertexs[i] = m_OriginVertexs[i];
		m_Vertexs[i].Position[0] += m_BlueTransition.x;
		m_Vertexs[i].Position[1] += m_BlueTransition.y;
	}

	for (int i = RedCount; i < BlueCount + RedCount; i++)
	{
		m_Vertexs[i] = m_OriginVertexs[i];
		m_Vertexs[i].Position[0] += m_RedTransition.x;
		m_Vertexs[i].Position[1] += m_RedTransition.y;
	}

	// ���ö�̬������
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_Vertexs), m_Vertexs);

}

void TestModule::TestDynamicDraw::OnRender()
{
	static glm::mat4 mvp = glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f);

	glClear(GL_COLOR_BUFFER_BIT);

	GL_CALL(glBindVertexArray(m_VAO));
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB));
	m_Shader.Bind();
	m_Shader.SetUniformMat4f("u_MVP", mvp);

	GL_CALL(glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, nullptr));
}

void TestModule::TestDynamicDraw::OnImGuiRender()
{
	ImGui::Begin("ClearColor");
	ImGui::SliderFloat2("Blue Block", &m_BlueTransition.x, -2.0f, 2.0f);
	ImGui::SliderFloat2("Red Block", &m_RedTransition.x, -2.0f, 2.0f);
	ImGui::End();
}
