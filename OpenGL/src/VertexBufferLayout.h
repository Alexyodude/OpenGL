#pragma once

#include <iostream>
#include <vector>

#include "Log.h"

struct VertexBufferElement // vertex buffer element
{
	unsigned int type; // type
	unsigned int count; // count
	unsigned char normalized; // normalized

	static unsigned int GetSizeOfType(unsigned int type) // get size of type
	{
		switch (type) // switch type
		{
		case GL_FLOAT: return 4; // float
		case GL_UNSIGNED_INT: return 4; // unsigned int
		case GL_UNSIGNED_BYTE: return 1; // unsigned byte
		}
		ASSERT(false); // assert false
		return 0; // return 0
	}
};

class VertexBufferLayout
{
private:
	std::vector<VertexBufferElement> m_Elements; // elements
	unsigned int m_Stride; // stride
public:
	VertexBufferLayout() // constructor
		: m_Stride(0) {} // initialize stride to 0

	template<typename T>
	void Push(unsigned int count) // push
	{
		std::runtime_error(false);
	}

	template<>
	void Push<float>(unsigned int count) // push float
	{
		m_Elements.push_back({ GL_FLOAT, count, GL_FALSE }); // push back element
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT); // increment stride
	}

	template<>
	void Push<unsigned int>(unsigned int count) // push unsigned int
	{
		m_Elements.push_back({ GL_UNSIGNED_INT, count, GL_FALSE }); // push back element
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT); // increment stride
	}

	template<>
	void Push<unsigned char>(unsigned int count) // push unsigned char
	{
		m_Elements.push_back({ GL_UNSIGNED_BYTE, count, GL_TRUE }); // push back element
		m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE); // increment stride
	}

	inline const std::vector<VertexBufferElement> GetElements() const { return m_Elements; } // get elements
	inline unsigned int GetStride() const { return m_Stride; } // get stride
};

