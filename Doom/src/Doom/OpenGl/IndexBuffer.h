#pragma once

namespace Doom {

	class DOOM_API IndexBuffer {
	public:

		unsigned int m_RendererID = UINT_MAX;
		unsigned int m_count = 0;

		IndexBuffer() {}
		~IndexBuffer();

		void Init(const uint32_t* data, uint32_t count);

		void Clear();

		void Bind() const;
		void UnBind() const;
	};

}