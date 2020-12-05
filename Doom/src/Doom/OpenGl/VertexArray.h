#pragma once

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"

namespace Doom {

	class DOOM_API VertexArray {
	private:

		unsigned int m_RendererID;
	public:

		VertexArray();
		~VertexArray();

		void Bind() const;
		void UnBind() const;
		inline unsigned int GetVertexArray() { return m_RendererID; }
		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout, const GLuint offsetOfGlVertexAttribArray = 0,const GLuint devisor = 0);
	};

}