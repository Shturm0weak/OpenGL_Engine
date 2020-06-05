#ifndef INPUT_H
#define INPUT_H

namespace Doom {

	class DOOM_API Input {
	public:
		inline static bool IsKeyPressed(int keycode) { return s_Instance->IsKeyPressedimp(keycode); }
		inline static bool IsMousePressed(int keycode) { return s_Instance->IsMousePressedimp(keycode); }
		inline static bool IsMouseDown(int keycode) { return s_Instance->IsMousePressedDownimp(keycode); }
		inline static bool IsKeyDown(int keycode) { return s_Instance->IsKeyPressedDownimp(keycode); }
		
		//Clear vector of pressed buttons for one frame
		//So it needs to not check every time if is 
		//button pressed, it just goes through vector
		//and see whether button was pressed for this frame or not
		inline static void Clear() { s_Instance->Clearimp(); }
	protected:
		virtual void Clearimp() = 0;
		virtual bool IsKeyPressedimp(int keycode) = 0;
		virtual bool IsMousePressedimp(int keycode) = 0;
		virtual bool IsMousePressedDownimp(int keycode) = 0;
		virtual bool IsKeyPressedDownimp(int keycode) = 0;
	private:
		static Input* s_Instance;
	};

}

#endif