#pragma once

namespace Doom {

	class DOOM_API VertexBuffer {
	private:
		unsigned int m_RendererID;
	public:
		VertexBuffer(const void* data, unsigned int size, bool Static = true);
		~VertexBuffer();

		void Bind()const;
		void UnBind()const;
		unsigned int& GetVertexBuffer() { return m_RendererID; }
	};

}