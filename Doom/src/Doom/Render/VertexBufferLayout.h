#pragma once
#include <glew.h>
 
namespace Doom {

	struct DOOM_API VertexBufferElement {
		unsigned int type;
		unsigned int count;
		unsigned int normalized;

		static unsigned int GetSizeOfType(unsigned int type) {
			switch (type)
			{
			case GL_FLOAT:			return 4;
			case GL_UNSIGNED_INT:	return 4;
			case GL_UNSIGNED_BYTE:	return 1;
			case GL_DOUBLE:			return 8;
			}
			return 0;
		}
	};

	class DOOM_API VertexBufferLayout {
	private:
		std::vector<VertexBufferElement> m_Elements;
		unsigned int m_Stride;
	public:
		VertexBufferLayout()
			:m_Stride(0) {}

		inline void Clear() { m_Stride = 0; m_Elements.clear(); }

		template<typename T>
		void Push(unsigned int count) {
			static_assert(false);
		}

		template<>
		void Push<float>(unsigned int count) {
			m_Elements.push_back({ GL_FLOAT, count ,GL_FALSE });
			m_Stride += count * VertexBufferElement::GetSizeOfType(GL_FLOAT);
		}

		template<>
		void Push<double>(unsigned int count) {
			m_Elements.push_back({ GL_DOUBLE, count ,GL_FALSE });
			m_Stride += count * VertexBufferElement::GetSizeOfType(GL_DOUBLE);
		}

		template<>
		void Push<unsigned int>(unsigned int count) {
			m_Elements.push_back({ GL_UNSIGNED_INT,count,GL_FALSE });
			m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_INT);
		}
		template<>
		void Push<unsigned char>(unsigned int count) {
			m_Elements.push_back({ GL_UNSIGNED_BYTE,count,GL_TRUE });
			m_Stride += count * VertexBufferElement::GetSizeOfType(GL_UNSIGNED_BYTE);
		}

		inline const std::vector<VertexBufferElement> GetElements() const& { return m_Elements; }
		inline unsigned int GetStride() const { return m_Stride; }
	};

}