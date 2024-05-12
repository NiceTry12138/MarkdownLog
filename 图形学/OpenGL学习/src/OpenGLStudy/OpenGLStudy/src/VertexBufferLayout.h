#pragma once
#include <vector>
#include <GL/glew.h>

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	bool normalized;
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements;

public:
	VertexBufferLayout();
	~VertexBufferLayout();

	template<typename T>
	void Push(unsigned int count) {
		static_assert(false);
	}

	template<>
	void Push<float>(unsigned int count) {
		m_Elements.push_back({GL_FLOAT, count, false});
	}

	template<>
	void Push<int>(unsigned int count) {
		m_Elements.push_back({ GL_UNSIGNED_INT, count, false });
	}
};

