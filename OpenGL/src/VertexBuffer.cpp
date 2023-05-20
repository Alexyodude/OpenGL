#include "VertexBuffer.h"
#include "Log.h"

VertexBuffer::VertexBuffer(const void* data, unsigned int size)
{
	GLCall(glGenBuffers(1, &m_RendererID)); // generate buffer
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // bind buffer
	GLCall(glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW)); // fill buffer with data
}

VertexBuffer::~VertexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_RendererID)); // delete buffer
}

void VertexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // bind buffer
}

void VertexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, 0)); // unbind buffer
}

void VertexBuffer::SetData(const void* data, unsigned int size)
{
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_RendererID)); // bind buffer
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, size, data)); // fill buffer with data
}