#pragma once

namespace Doom {

	class DOOM_API VertexBuffer {
	private:

		unsigned int m_RendererID;
	public:

		VertexBuffer() {}
		VertexBuffer(const void* data, unsigned int size, bool Static = true);
		~VertexBuffer();

		void Bind()const;
		void UnBind()const;
		void Invalidate();
		unsigned int& GetVertexBuffer() { return m_RendererID; }
	};

}