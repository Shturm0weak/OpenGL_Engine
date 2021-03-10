#pragma once

namespace Doom {

	class DOOM_API VertexBuffer {
	public:

		unsigned int m_RendererID = UINT_MAX;

		VertexBuffer() {}
		~VertexBuffer();

		void Init(const void* data, unsigned int size, bool Static = true);

		void Bind()const;
		void UnBind()const;
		void Invalidate();
	};

}