#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace Doom {

	class DOOM_API VertexArray {
	public:

		unsigned int m_RendererID = UINT32_MAX;

		VertexArray() {}
		~VertexArray();

		void Init();

		void Clear();

		void Bind() const;
		void UnBind() const;
		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, const GLuint offsetOfGlVertexAttribArray = 0,const GLuint devisor = 0);
	};

}