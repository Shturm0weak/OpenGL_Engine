#pragma once

namespace Doom {

	struct DOOM_API FrameBufferParams {
		uint32_t m_Width = 800;
		uint32_t m_Height = 600;
		int m_FrameBufferAttachment = GL_COLOR_ATTACHMENT0;
		int m_TextureInternalFormat = GL_RGBA;
		int TextureType = GL_UNSIGNED_BYTE;
		bool m_HasRBO = false;
		bool m_ReadBuffer = true;
		bool m_DrawBuffer = true;
		uint32_t m_TexturesAmount = 1;
	};

	class DOOM_API FrameBuffer {
	public:

		bool m_HasRbo = false;
		glm::vec2 size;
		unsigned int m_Rbo = 0;
		unsigned int m_Fbo = 0;
		std::vector<unsigned int> m_Textures;

		FrameBuffer(const FrameBufferParams& params);
		~FrameBuffer();

		void Resize(uint32_t width, uint32_t height);
		void Bind() const;
		void UnBind() const;
	};

}