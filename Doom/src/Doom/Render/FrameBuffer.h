#pragma once

namespace Doom {

	class DOOM_API FrameBuffer {
	private:
	public:
		unsigned int fbo = 0;
		unsigned int texture = 0;

		FrameBuffer();
		~FrameBuffer();

		void Bind();
		void UnBind();
	};
}