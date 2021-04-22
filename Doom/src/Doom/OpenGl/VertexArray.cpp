#include "../pch.h"
#include "VertexArray.h"

using namespace Doom;

void VertexArray::Init()
{
	glGenVertexArrays(1, &m_RendererID);
	Bind();
}

void Doom::VertexArray::Clear()
{
	m_RendererID = UINT32_MAX;
	glDeleteVertexArrays(1, &m_RendererID);
}

VertexArray::~VertexArray()
{
	Clear();
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout,const GLuint offsetOfGlVertexAttribArray,const GLuint devisor) 
{
	Bind();
	vb.Bind();
	const auto& elements = layout.m_Elements;
	unsigned int offset = 0;
	for(unsigned int i = 0;i < elements.size();i++)
	{
		const auto& element = elements[i];
		glEnableVertexAttribArray(i + offsetOfGlVertexAttribArray);
		glVertexAttribPointer(i + offsetOfGlVertexAttribArray, element.m_Count, element.m_Type, element.m_Normalized,layout.m_Stride, (const void*)offset);
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

