#include "../pch.h"
#include "VertexArray.h"

using namespace Doom;

VertexArray::VertexArray()
{
	glGenVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray()
{
	glDeleteVertexArrays(1, &m_RendererID);
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout,const GLuint offsetOfGlVertexAttribArray,const GLuint devisor) 
{
	Bind();
	vb.Bind();
	const auto& elements = layout.GetElements();
	unsigned int offset = 0;
	for(unsigned int i = 0;i < elements.size();i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i + offsetOfGlVertexAttribArray);
		glVertexAttribPointer(i + offsetOfGlVertexAttribArray, element.m_Count, element.m_Type, element.m_Normalized,layout.GetStride(), (const void*)offset);
		glVertexAttribDivisor(i + offsetOfGlVertexAttribArray,devisor);
		offset += element.m_Count * VertexBufferElement::GetSizeOfType(element.m_Type);
	}
}

void VertexArray::Bind() const
{
	glBindVertexArray(m_RendererID);
}

void VertexArray::UnBind() const
{
	glBindVertexArray(0);
}

