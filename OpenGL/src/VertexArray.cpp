#include "Log.h"

#include "VertexArray.h"
#include "VertexBufferLayout.h"

VertexArray::VertexArray()
{
	GLCall(glGenVertexArrays(1, &m_RendererID)); // generate vertex array
}

VertexArray::~VertexArray()
{
	GLCall(glDeleteVertexArrays(1, &m_RendererID)); // delete vertex array
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout)
{
	Bind(); // bind vertex array
	vb.Bind(); // bind vertex buffer
	const auto& elements = layout.GetElements(); // get elements
	unsigned int offset = 0; // offset
	for (unsigned int i = 0; i < elements.size(); i++) // for each element
	{
		const auto& element = elements[i]; // get element
		GLCall(glEnableVertexAttribArray(i)); // enable vertex attribute array
		//std :: cout << "element.count: " << element.count << std::endl;
		//std::cout << "element.type: " << element.type << std::endl;
		//std::cout << "element.normalized: " << element.normalized << std::endl;
		//std::cout << "layout.GetStride(): " << layout.GetStride() << std::endl;
		//std::cout << "offset: " << offset << std::endl;
		GLCall(glVertexAttribPointer(i, element.count, element.type, 
			element.normalized, layout.GetStride(), (const void*)offset)); // set vertex attribute pointer
		
		offset += element.count * VertexBufferElement::GetSizeOfType(element.type); // increment offset
	}
}

void VertexArray::Bind() const
{
	GLCall(glBindVertexArray(m_RendererID)); // bind vertex array
}

void VertexArray::Unbind() const
{
	GLCall(glBindVertexArray(0)); // unbind vertex array
}