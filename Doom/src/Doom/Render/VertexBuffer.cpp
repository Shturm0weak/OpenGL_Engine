#include "../pch.h"
#include "VertexBuffer.h"

using namespace Doom;

VertexBuffer::VertexBuffer(const void* data, unsigned int size,bool Static) {
	glGenBuffers(1, &m_RendererID);
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
	if(Static)
		glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
	else
		glBufferData(GL_ARRAY_BUFFER, size, NULL, GL_DYNAMIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
	glDeleteBuffers(1, &m_RendererID);
}

void VertexBuffer::Bind() const {
	glBindBuffer(GL_ARRAY_BUFFER, m_RendererID);
}

void VertexBuffer::UnBind() const {
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Doom::VertexBuffer::Invalidate()
{
	glInvalidateBufferData(m_RendererID);
}
