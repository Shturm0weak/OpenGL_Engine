#ifndef INPUT_H
#define INPUT_H

namespace Doom {

	class DOOM_API Input {
	public:
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedimp(keycode); }
		inline static bool IsMousePressed(int keycode) { return s_Instance->IsMousePressedimp(keycode); }
	protected:
		virtual bool IsKeyPressedimp(int keycode) = 0;
		virtual bool IsMousePressedimp(int keycode) = 0;
	private:
		static Input* s_Instance;
	};

}

#endif