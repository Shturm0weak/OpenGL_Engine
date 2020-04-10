#pragma once
#include "../Render/Text.h"
#include "../Gui/GuiItem.h"

namespace Doom {

	class DOOM_API Button : public GuiItem {
	public:
		Button(Font* font, std::string label, float x = 0, float y = 0, float width = 1, float height = 1, float textScale = 8, float scale = 1) {
			x /= 5.f;
			y /= 7.f;
			mesh2D[0] = -width / 2.;
			x += width / 2.;
			mesh2D[4] = width / 2.;
			mesh2D[8] = width / 2.;
			mesh2D[12] = -width / 2.;
			mesh2D[1] = -height / 2.;
			mesh2D[5] = -height / 2.;
			mesh2D[9] = height / 2.;
			mesh2D[13] = height / 2.;
			_scale.x = scale;
			_scale.y = scale;
			mesh2D[0] *= _scale.x;
			mesh2D[1] *= _scale.x;
			mesh2D[4] *= _scale.x;
			mesh2D[5] *= _scale.x;
			mesh2D[8] *= _scale.x;
			mesh2D[9] *= _scale.x;
			mesh2D[12] *= _scale.x;
			mesh2D[13] *= _scale.x;
			mesh2D[0] += x;
			mesh2D[1] += y;
			mesh2D[4] += x;
			mesh2D[5] += y;
			mesh2D[8] += x;
			mesh2D[9] += y;
			mesh2D[12] += x;
			mesh2D[13] += y;

			Batch::GetInstance()->Submit(*(GuiItem*)this);

			float offset = (abs(mesh2D[0] - mesh2D[4]) - (label.size() / 7.) * textScale / 10) * 52.;
			Gui::GetInstance()->Text(font,label,true,-4.5,-3,32);
		}

		bool IsPressed();
	};

}