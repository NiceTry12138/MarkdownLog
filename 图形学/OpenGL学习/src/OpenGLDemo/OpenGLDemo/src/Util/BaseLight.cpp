#include "BaseLight.h"

BaseLight::BaseLight()
{
	//					 坐标					顶点法线	
	m_vertexs.push_back({ 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f });
	m_vertexs.push_back({ 0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f });
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f });
	m_vertexs.push_back({ 0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f });
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f, 0.0f, 0.0f, -1.0f });
	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f });

	m_vertexs.push_back({ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ 0.5f,  0.5f, -0.5f, 1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ 0.5f,  0.5f,  0.5f, 1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ 0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ 0.5f, -0.5f,  0.5f, 1.0f, 0.0f, 0.0f });

	m_vertexs.push_back({ 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, });
	m_vertexs.push_back({ 0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, });
	m_vertexs.push_back({ 0.5f, -0.5f,  0.5f, 0.0f, -1.0f, 0.0f, });

	m_vertexs.push_back({ 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, });
	m_vertexs.push_back({ 0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f, 0.0f, 1.0f, 0.0f, });
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, });
	m_vertexs.push_back({ 0.5f,  0.5f,  0.5f, 0.0f, 1.0f, 0.0f, });

	m_vertexs.push_back({ 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f });
	m_vertexs.push_back({ 0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f });
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f });
	m_vertexs.push_back({ 0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f });
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f, 0.0f, 0.0f, 1.0f });
	m_vertexs.push_back({ -0.5f, -0.5f,  0.5f, 0.0f, 0.0f, 1.0f });

	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ -0.5f,  0.5f, -0.5f, -1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ -0.5f,  0.5f,  0.5f, -1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f });
	m_vertexs.push_back({ -0.5f, -0.5f,  0.5f, -1.0f, 0.0f, 0.0f });

}

void BaseLight::Draw()
{
	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6 * 6);
}

glm::vec3 BaseLight::GetLocation()
{
	return m_Location;
}

void BaseLight::SetLocation(const glm::vec3 inLocation)
{
	m_Location = inLocation;
}

void BaseLight::Init()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, m_vertexs.size() * sizeof(Vertex_v1), m_vertexs.data(), GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_v1), (void*)offsetof(Vertex_v1, position));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex_v1), (void*)offsetof(Vertex_v1, normal));

	glBindVertexArray(GL_ZERO);
}
