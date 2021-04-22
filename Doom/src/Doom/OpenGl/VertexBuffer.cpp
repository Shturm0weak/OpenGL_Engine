#include "../pch.h"
#include "VertexBuffer.h"

using namespace Doom;

void VertexBuffer::Init(const void* data, unsigned int size, bool Static)
{
	glGenBuffers(1, &m_RendererID);
	Bind();
	if (Static)
	{
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	}
	else
	{
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
	}
}

void VertexBuffer::Clear()
{
	m_RendererID = UINT32_MAX;
	glDeleteBuffers(1, &m_RendererID);
}

VertexBuffer::~VertexBuffer()
{
	Clear();
}

void VertexBuffer::Bind() const 
{
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::UnBind() const
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexBuffer::Invalidate() const
{
	glInvalidateBufferData(m_RendererID);
}