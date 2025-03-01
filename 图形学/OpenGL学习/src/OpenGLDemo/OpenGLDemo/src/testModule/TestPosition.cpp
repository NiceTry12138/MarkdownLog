#include "TestPosition.h"

void TestPosition::OnEnter(GLFWwindow* window)
{
	//					 坐标					颜色						UV 坐标			贴图序号
	m_vertexs.push_back({ 0.5f,  0.5f, 0.0f,	1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f,		0 });
	m_vertexs.push_back({ 0.5f, -0.5f, 0.0f,	1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 0.0f,		0 });
	m_vertexs.push_back({ -0.5f, -0.5f, 0.0f,	0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		1 });
	m_vertexs.push_back({ -0.5f,  0.5f, 0.0f,	0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,		1 });

	GLuint indices[] = {
		0, 1, 2,
		2, 3, 0
	};

	// 创建 VAO
	glGenVertexArrays(1, &m_VAO);

	// 绑定 VAO
	glBindVertexArray(m_VAO);

	// 创建 VBO IB
	glGenBuffers(1, &m_VBO);
	glGenBuffers(1, &m_IB);

	// 绑定 VBO 和 数据
	GL_CALL(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GL_CALL(glBufferData(GL_ARRAY_BUFFER, m_vertexs.size() * sizeof(Vertex_v0), m_vertexs.data(), GL_STATIC_DRAW));

	// 绑定 IB 和 数据
	GL_CALL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_IB));
	GL_CALL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	// 设置 VAO 内存结构
	GL_CALL(glEnableVertexAttribArray(0));
	GL_CALL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_v0), (void *)offsetof(Vertex_v0, position)));

	GL_CALL(glEnableVertexAttribArray(1));
	GL_CALL(glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex_v0), (void*)offsetof(Vertex_v0, color)));

	GL_CALL(glEnableVertexAttribArray(2));
	GL_CALL(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex_v0), (void*)offsetof(Vertex_v0, texCoord)));

	GL_CALL(glEnableVertexAttribArray(3));
	GL_CALL(glVertexAttribPointer(3, 1, GL_INT, GL_FALSE, sizeof(Vertex_v0), (void*)offsetof(Vertex_v0, texIndex)));

	// 初始化 shader
	//m_Shader.NewInit("res/shader/TextPosition/Vertex.vert", "res/shader/TextPosition/Fragment.frag");
	m_Shader.Init("res/shader/TextPosition/Vertex.vert", "res/shader/TextPosition/Fragment.frag");

	// 初始化 texture
	m_Tex1.Init("res/textures/test2.png");
	m_Tex2.Init("res/textures/test3.png");

	m_Tex1.Bind(0);
	m_Tex2.Bind(1);

	m_Shader.UnBind();
}

void TestPosition::OnExit(GLFWwindow* window)
{
	
}

void TestPosition::UpdateLogic(float delayTime)
{
	
}

void TestPosition::ClearRender(GLFWwindow* window)
{
	TestBase::ClearRender(window);
}

void TestPosition::Render(GLFWwindow* window)
{
	m_Shader.Bind();
	m_Shader.SetUniform1i("u_Texture0", 0);
	m_Shader.SetUniform1i("u_Texture1", 1);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void TestPosition::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("ClearColor");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	if (ImGui::Button("Close Window"))
		glfwSetWindowShouldClose(window, true);
	ImGui::End();
}
