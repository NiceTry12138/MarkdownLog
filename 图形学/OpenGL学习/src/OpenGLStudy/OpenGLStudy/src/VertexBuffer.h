#pragma once
#include <GL/glew.h>

class VertexBuffer
{
private:
	GLuint m_RendererID;

public:
	VertexBuffer(const void* data, GLuint size);
	VertexBuffer() = default;
	~VertexBuffer();

	void Bind() const;
	void UnBind() const;
};
