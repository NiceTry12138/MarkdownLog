#pragma once
#include "CommonData.h"

class BaseLight
{
public:
	BaseLight();

	void Draw();

	glm::vec3 GetLocation();
	void SetLocation(const glm::vec3 inLocation);

	void Init();

protected:
private:
	glm::vec3 m_Location = glm::vec3(3.0f);

	std::vector<Vertex_v1> m_vertexs;

	GLuint VAO{ GL_ZERO };
	GLuint VBO{ GL_ZERO };
};

