#include "../pch.h"
#include "IndexBuffer.h"

using namespace Doom;

void Doom::IndexBuffer::Init(const uint32_t * data, uint32_t count)
{
	m_count = count;
	glGenBuffers(1, &m_RendererID);
	Bind();
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(uint32), data, GL_DYNAMIC_DRAW);
}

Doom::IndexBuffer::~IndexBuffer()
{
	glDeleteBuffers(1, &m_RendererID);
}

void Doom::IndexBuffer::Bind() const
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_RendererID);
}

void Doom::IndexBuffer::UnBind() const 
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}