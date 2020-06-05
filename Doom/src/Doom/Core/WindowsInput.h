#pragma once
#include "Input.h"
#include <map>

namespace Doom {

	class DOOM_API WindowsInput : public Input {
	protected:
		std::map<int,double> pressedButtonsTime;
		std::map<int, bool> pressedMouseButtons;
		std::map<int, bool> pressedKeysButtons;
		virtual void Clearimp() override;
		virtual bool IsKeyPressedimp(int keycode) override;
		virtual bool IsMousePressedimp(int keycode) override;
		virtual bool IsMousePressedDownimp(int keycode) override;
		virtual bool IsKeyPressedDownimp(int keycode) override;
	};

}