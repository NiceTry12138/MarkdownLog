#pragma once
#include "Renderer.h"

class VertexBuffer;
class VertexBufferLayout;

class VertexArray
{
private:
	GLuint m_RendererId;

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);

	void Bind();
	void Unbind();
};

