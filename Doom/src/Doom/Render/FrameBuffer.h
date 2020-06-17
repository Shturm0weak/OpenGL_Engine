#pragma once

namespace Doom {

	class DOOM_API FrameBuffer {
	private:
	public:
		unsigned int rbo = 0;
		unsigned int fbo = 0;
		unsigned int texture = 0;

		FrameBuffer();
		~FrameBuffer();

		void Resize(float width,float height);
		void Bind();
		void UnBind();
	};
}