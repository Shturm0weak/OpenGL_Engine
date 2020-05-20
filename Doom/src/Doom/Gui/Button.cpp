#pragma once
#include "Button.h"
#include "../Render/ViewPort.h"

using namespace Doom;

bool Button::IsPressed() {
	if (ViewPort::Instance()->GetMousePositionToScreenSpace().x > -2.5 && ViewPort::Instance()->GetMousePositionToScreenSpace().x < 2.5
		&& ViewPort::Instance()->GetMousePositionToScreenSpace().y > -3.3 && ViewPort::Instance()->GetMousePositionToScreenSpace().y < 0) {
		return true;
	}
	else {
		return false;
	}
}

