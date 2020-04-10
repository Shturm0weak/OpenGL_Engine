#pragma once
#include "Button.h"

using namespace Doom;

bool Button::IsPressed() {
	if (Window::GetMousePositionToScreenSpace().x > -2.5 && Window::GetMousePositionToScreenSpace().x < 2.5
		&& Window::GetMousePositionToScreenSpace().y > -3.3 && Window::GetMousePositionToScreenSpace().y < 0) {
		return true;
	}
	else {
		return false;
	}
}

