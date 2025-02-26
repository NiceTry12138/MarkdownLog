#include "TestDynamicDraw.h"

void TestModule::TestDynamicDraw::Init()
{
	// 初始化顶点信息 坐标、  颜色、  TexCoord、  序号
	m_OriginVertexs[0] = { -1.5f, -0.5f, 0.0f,    0.1f, 0.6f, 0.9f, 1.0f,    0.0f, 0.0f,    0.0f };
	m_OriginVertexs[1] = { -0.5f, -0.5f, 0.0f,    0.1f, 0.6f, 0.9f, 1.0f,    1.0f, 0.0f,    0.0f };
	m_OriginVertexs[2] = { -0.5f,  0.5f, 0.0f,    0.1f, 0.6f, 0.9f, 1.0f,    1.0f, 1.0f,    0.0f };
	m_OriginVertexs[3] = { -1.5f,  0.5f, 0.0f,    0.1f, 0.6f, 0.9f, 1.0f,    0.0f, 1.0f,    0.0f };
	m_OriginVertexs[4] = {  0.5f, -0.5f, 0.0f,    0.9f, 0.6f, 0.1f, 1.0f,    0.0f, 0.0f,    1.0f };
	m_OriginVertexs[5] = {  1.5f, -0.5f, 0.0f,    0.9f, 0.6f, 0.1f, 1.0f,    1.0f, 0.0f,    1.0f };
	m_OriginVertexs[6] = {  1.5f,  0.5f, 0.0f,    0.9f, 0.6f, 0.1f, 1.0f,    1.0f, 1.0f,    1.0f };
	m_OriginVertexs[7] = {  0.5f,  0.5f, 0.0f,    0.9f, 0.6f, 0.1f, 1.0f,    0.0f, 1.0f,    1.0f };

	// 创建并绑定 VAO
	glCreateVertexArrays(1, &m_VAO);
	glBindVertexArray(m_VAO);

	// 创建并绑定 VBO
	glCreateBuffers(1, &m_VBO);
	glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * 1000, nullptr, GL_DYNAMIC_DRAW);	// 预先申请好 1000 个 Vertex 的内存地址 虽然可能用不到

	// 设置 VAO 内存结构
	GL_CALL(glEnableVertexArrayAttrib(m_VAO, 0));
	// 绑定坐标 序号为 0， 坐标是 3 个 GL_FLOAT, 不需要归一化，步长大小是 sizeof(Vertex) 位置偏移是 offsetof(Vertex, Position)
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position)));

	GL_CALL(glEnableVertexArrayAttrib(m_VAO, 1));
	GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Color)));

	GL_CALL(glEnableVertexArrayAttrib(m_VAO, 2));
	GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TexCoord)));

	GL_CALL(glEnableVertexArrayAttrib(m_VAO, 3));
	GL_CALL(glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, TextureIndex)));

	// 顶点索引数组 
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

	// 设置动态缓冲区
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
