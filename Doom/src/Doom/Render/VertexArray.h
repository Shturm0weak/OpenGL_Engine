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

		void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout& layout);
	};

}