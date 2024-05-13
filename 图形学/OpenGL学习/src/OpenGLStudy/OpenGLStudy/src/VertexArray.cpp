#include "VertexArray.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexBufferLayout.h"

VertexArray::VertexArray()
{
	GL_CALL(glGenVertexArrays(1, &m_RendererId));
}

VertexArray::~VertexArray()
{
	GL_CALL(glDeleteVertexArrays(1, &m_RendererId));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind();
	vb.Bind();

	const auto& elements = layout.GetElements();
	unsigned int offset = 0;

	for (unsigned int index = 0; index < elements.size(); ++index) {
		const auto& element = elements[index];
		GL_CALL(glEnableVertexAttribArray(0));
		GL_CALL(glVertexAttribPointer(index, element.count, element.type, element.normalized, layout.GetStride(), (const void*)offset));
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type);
	}
}

void VertexArray::Bind()
{
	GL_CALL(glBindVertexArray(m_RendererId));
}

void VertexArray::Unbind()
{
	GL_CALL(glBindVertexArray(0));
}
