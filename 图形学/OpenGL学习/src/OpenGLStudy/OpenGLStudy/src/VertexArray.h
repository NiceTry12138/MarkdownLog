#pragma once

class VertexBuffer;
class VertexBufferLayout;

class VertexArray
{
private:

public:
	VertexArray();
	~VertexArray();

	void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
};

