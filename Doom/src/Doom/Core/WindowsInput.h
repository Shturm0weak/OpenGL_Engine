#pragma once
#include "Input.h"

namespace Doom {

	class DOOM_API WindowsInput : public Input {
	protected:
		virtual bool IsKeyPressedimp(int keycode) override;
		virtual bool IsMousePressedimp(int keycode) override;
	};

}