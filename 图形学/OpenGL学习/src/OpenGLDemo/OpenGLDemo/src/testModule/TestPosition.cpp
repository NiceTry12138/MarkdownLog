#include "TestPosition.h"
#include "../Util/RenderSettings.h"

void TestPosition::OnEnter(GLFWwindow* window)
{
	//					 坐标					颜色						UV 坐标			贴图序号
	m_vertexs.push_back({ 0.5f,  0.5f, -0.5f,	1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f,		0 });	// 立方体 下面 右上角
	m_vertexs.push_back({ 0.5f, -0.5f, -0.5f,	1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 0.0f,		0 });	// 立方体 下面 右下角
	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f,	0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		1 });	// 立方体 下面 左下角
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f,	0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,		1 });	// 立方体 下面 左上角
	m_vertexs.push_back({ 0.5f,  0.5f,  0.5f,	1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 1.0f,		0 });	// 立方体 上面 右上角
	m_vertexs.push_back({ 0.5f, -0.5f,  0.5f,	1.0f, 1.0f, 0.0f, 1.0f,		1.0f, 0.0f,		0 });	// 立方体 上面 右下角
	m_vertexs.push_back({ -0.5f, -0.5f,  0.5f,	0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 0.0f,		1 });	// 立方体 上面 左下角
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f,	0.0f, 1.0f, 1.0f, 1.0f,		0.0f, 1.0f,		1 });	// 立方体 上面 左上角

	GLuint indices[] = {
		0, 1, 2, 2, 3, 0,			// 组成下面的两个三角形
		4, 5, 6, 6, 7, 4,			
		1, 2, 6, 6, 5, 1,
		0, 1, 5, 5, 4, 0,
		2, 3, 7, 7, 6, 2,
		0, 3, 4, 4, 7, 3,
	};
	
	// 启动深度测试
	glEnable(GL_DEPTH_TEST);

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

	//m_model = glm::rotate(m_model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//m_view = glm::translate(m_view, m_Transition);
	//m_proj = glm::perspective(glm::radians(45.0f), (float)RSI->ViewportWidth / (float)RSI->ViewportWidth, 0.1f, 100.0f);
}

void TestPosition::OnExit(GLFWwindow* window)
{
	
}

void TestPosition::UpdateLogic(float delayTime)
{
	glm::vec3 RotateAxis = glm::vec3(0.0f, 0.0f, 0.0f);
	RotateAxis.r = m_RotateX ? 1.0f : 0.0f;
	RotateAxis.g = m_RotateY ? 1.0f : 0.0f;
	RotateAxis.b = m_RotateZ ? 1.0f : 0.0f;

	m_Rotate += delayTime * m_RotateSpeed;
	if (m_Rotate > 180.0f) {
		m_Rotate -= 360;
	}

	m_model = glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotate), RotateAxis);
	m_view = glm::translate(glm::mat4(1.0f), m_Transition);

	// 可能会更新窗口视口大小 每帧更新一下
	m_proj = glm::perspective(glm::radians(45.0f), (float)RSI->ViewportHeight / (float)RSI->ViewportWidth, 0.1f, 100.0f);
}

void TestPosition::ClearRender(GLFWwindow* window)
{
	//TestBase::ClearRender(window);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void TestPosition::Render(GLFWwindow* window)
{
	m_Shader.Bind();
	m_Shader.SetUniform1i("u_Texture0", 0);
	m_Shader.SetUniform1i("u_Texture1", 1);

	m_Shader.SetUniformMat4f("model", m_model);
	m_Shader.SetUniformMat4f("view", m_view);
	m_Shader.SetUniformMat4f("projection", m_proj);

	glBindVertexArray(m_VAO);
	glDrawElements(GL_TRIANGLES, 6 * 6, GL_UNSIGNED_INT, 0);
}

void TestPosition::UpdateImGUI(GLFWwindow* window)
{
	const auto& io = ImGui::GetIO();

	ImGui::Begin("Rotate");

	ImGui::Checkbox("Rotate X", &m_RotateX);
	ImGui::Checkbox("Rotate Y", &m_RotateY);
	ImGui::Checkbox("Rotate Z", &m_RotateZ);
	ImGui::SliderFloat("Rotate Speed", &m_RotateSpeed, 1.0f, 500);
	ImGui::SliderFloat3("Camera Location", &m_Transition.r, -5.0f, 5.0f);

	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
	if (ImGui::Button("Close Window"))
		glfwSetWindowShouldClose(window, true);
	ImGui::End();
}
