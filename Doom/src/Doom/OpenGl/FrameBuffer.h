#pragma once

namespace Doom {

	class DOOM_API FrameBuffer {
	public:

		bool m_HasRbo = false;
		glm::vec2 size;
		unsigned int m_Rbo = 0;
		unsigned int m_Fbo = 0;
		std::vector<unsigned int> m_Textures;

		FrameBuffer(int width, int height, int TextureParameterinternalFormat, int TextureParametertextureType, int TextureParamLinear, int TextureParameterClamp, int FrameBufferAttachment, bool NeedRBO, bool ReadBuffer, bool DrawBuffer, int NumOfTextures = 1);
		~FrameBuffer();

		void Resize(float width, float height);
		void Bind();
		void UnBind();
	};

}