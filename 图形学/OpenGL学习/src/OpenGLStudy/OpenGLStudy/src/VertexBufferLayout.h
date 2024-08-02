#pragma once
#include <vector>
#include <GL/glew.h>

struct VertexBufferElement
{
	GLuint type;
	GLuint count;
	GLuint normalized;

	static GLuint GetSizeOfType(GLuint type) {
		switch (type)
		{
		case GL_FLOAT:
			return sizeof(GLfloat);
			break;
		case GL_UNSIGNED_INT:
			return sizeof(GLuint);
			break;
		case GL_UNSIGNED_BYTE:
			return sizeof(GLubyte);
			break;;
		}
		//ASSERT(false);
		return 0;
	}
};

typedef std::vector<VertexBufferElement> VBElemnts;

class VertexBufferLayout
{
private:
	VBElemnts m_Elements;
	GLuint m_Stride{ 0 };

public:

	template<typename T>
	void Push(unsigned int count) {
		//static_assert(false);
	}

	template<>
	void Push<GLfloat>(unsigned int count) {
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_FLOAT) * count;
	}

	template<>
	void Push<GLuint>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT) * count;
	}

	template<>
	void Push<GLubyte>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_FALSE });
		m_Stride += VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE) * count;
	}

	inline const VBElemnts GetElements() const { return m_Elements; }

	inline GLuint GetStride() const { return m_Stride; }
};

