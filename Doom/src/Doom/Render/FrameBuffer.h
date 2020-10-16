#pragma once

namespace Doom {

	class DOOM_API FrameBuffer {
	private:
	public:
		unsigned int rbo = 0;
		unsigned int fbo = 0;
		unsigned int texture = 0;

		glm::vec2 size;

		FrameBuffer(int width, int height, int TextureParameterinternalFormat, int TextureParametertextureType, bool TextureParameterClampBorder, int FrameBufferAttachment, bool NeedRBO, bool ReadBuffer, bool DrawBuffer);
		~FrameBuffer();

		void Resize(float width, float height);
		void Bind();
		void UnBind();
	};
}