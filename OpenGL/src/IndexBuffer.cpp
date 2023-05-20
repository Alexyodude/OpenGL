#include "IndexBuffer.h"
#include "Log.h"

IndexBuffer::IndexBuffer(const void* data, unsigned int count)
	: m_Count(count)
{
	ASSERT(sizeof(unsigned int) == sizeof(GLuint));

	GLCall(glGenBuffers(1, &m_RendererID)); // generate buffer
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID)); // bind buffer
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(unsigned int), data, GL_STATIC_DRAW)); // fill buffer with data
}

IndexBuffer::~IndexBuffer()
{
	GLCall(glDeleteBuffers(1, &m_RendererID)); // delete buffer
}

void IndexBuffer::Bind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID)); // bind buffer
}

void IndexBuffer::Unbind() const
{
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); // unbind buffer
}