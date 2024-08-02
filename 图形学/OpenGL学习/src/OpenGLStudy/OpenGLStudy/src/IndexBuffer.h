#pragma once
#include <GL/glew.h>

class IndexBuffer
{
private:
	GLuint m_RendererID;
	GLuint m_Count;

public:
	IndexBuffer(const GLuint* data, GLuint count);
	~IndexBuffer();

	void Bind() const;
	void Unbind() const;

	inline GLuint GetCount() const { return m_Count; }
};

