#pragma once

namespace Doom {

	class DOOM_API FrameBuffer {
	public:

		glm::vec2 size;
		unsigned int m_Rbo = 0;
		unsigned int m_Fbo = 0;
		unsigned int m_Texture = 0;

		FrameBuffer(int width, int height, int TextureParameterinternalFormat, int TextureParametertextureType, bool TextureParameterClampBorder, int FrameBufferAttachment, bool NeedRBO, bool ReadBuffer, bool DrawBuffer);
		~FrameBuffer();

		void Resize(float width, float height);
		void Bind();
		void UnBind();
	};

}