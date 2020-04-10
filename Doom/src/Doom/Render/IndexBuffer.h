#pragma once

namespace Doom {

	class DOOM_API IndexBuffer {
	private:
		unsigned int m_RendererID;
		unsigned int m_count;
	public:
		IndexBuffer(const unsigned int* data, unsigned int count);
		~IndexBuffer();

		void Bind() const;
		void UnBind() const;

		inline unsigned int GetCount() const { return m_count; }
	};

}