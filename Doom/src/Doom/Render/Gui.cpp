#include "Gui.h"
#include <functional>
#include "../Core/Timer.h"

using namespace Doom;

void Doom::Gui::Text(std::string str, int m_static, float x, float y, float startscale, glm::vec4 color, int charsAfterComma, ...)
{
	ApplyRelatedToPanelProperties(&x, &y);
	float scale = startscale / font->size;
	std::vector<Character*> characters;
	std::vector<unsigned int> newLines;
	float aRatio = Window::GetCamera().GetAspectRatio();
	float ratio = aRatio;
	int counter1 = 0;
	bool dotPass = false;
	va_list argptr;
	va_start(argptr, charsAfterComma);
	size_t strSize = str.size();
	for (size_t i = 0; i < strSize; i++)
	{
		char strChar = str[i];
		if (strChar == '%') {
			if (str[i + 1] == 'd')
				s = std::to_string(va_arg(argptr, int));
			else if (str[i + 1] == 'f') {
				s = std::to_string(va_arg(argptr, double));
			}
			else if (str[i + 1] == 's')
				s = (va_arg(argptr, std::string));
			size_t argSize = s.size();
			for (size_t j = 0; j < argSize; j++)
			{
				char pchar = s[j];
				if (pchar == '.' || pchar == ',') {
					dotPass = true;
					if (charsAfterComma == 0)
						break;
				}

				if(dotPass)
					counter1++;

				auto cIter = font->characters.find(pchar);
				if (cIter != font->characters.end()) {
					characters.push_back(cIter->second);
				}

				/*size_t size = font->characters.size();
				for (size_t i = 0; i < size; i++)
				{
					if (font->characters[i]->ch == pchar) {
						characters.push_back(font->characters[i]);
						break;
					}
				}*/

				if (counter1 > charsAfterComma)
					break;
			}
			dotPass = false;
			counter1 = 0;
			i++;
		}
		else if (strChar == '\n') {
			newLines.push_back(characters.size());
			continue;
		}
		else {
			auto cIter = font->characters.find(strChar);
			if (cIter != font->characters.end()) {
				characters.push_back(cIter->second);
			}

			/*size_t size = font->characters.size();
			for (size_t k = 0; k < size; k++)
			{
				if (font->characters[k]->ch == strChar) {
					characters.push_back(font->characters[k]);
					break;
				}
			}*/

		}
	}

	unsigned int size = characters.size();

	double _width = 0;
	double _height = 0;

	for (unsigned int i = 0; i < size; i++)
	{
		character = characters[i];
		character->_scale.x = scale;
		character->_scale.y = scale;
		float tempHeight = character->height * character->_scale.y;
		if (_height < tempHeight)
			_height = tempHeight;
		_width += character->xadvance * character->_scale.x;

	}

	if (xAlign == AlignHorizontally::XCENTER) {
		x -= _width * 0.5;
	}
	if (yAlign == AlignVertically::YCENTER) {
		y += _height * 0.5;
	}

	lastTextProperties.size = glm::vec2(_width, _height);
	lastTextProperties.pos = glm::vec2(x, y);

	if (IsRelatedToPanel)
		relatedPanelProperties.yOffset += _height + relatedPanelProperties.padding.y;

	unsigned int newLinesSize = newLines.size();
	for (unsigned int i = 0; i < size; i++)
	{
		for (unsigned int j = 0; j < newLinesSize; j++)
		{
			if (i == newLines[j]) {
				y -= startscale * 1.5;
				relatedPanelProperties.yOffset += _height + relatedPanelProperties.padding.y;
				characterXoffset = 0;
				newLines.erase(newLines.begin() + j);
				break;
			}
		}
		character = characters[i];
		character->isRelatedToCam = m_static;
		character->color = color;

		//Pos is top-left corner
		character->mesh2D[0] = 0;
		character->mesh2D[1] = -(ratio * (character->height + character->yoffset));
		character->mesh2D[4] = (ratio * (character->width));
		character->mesh2D[5] = -(ratio * (character->height + character->yoffset));
		character->mesh2D[8] = (ratio * (character->width));
		character->mesh2D[9] = -ratio * character->yoffset;
		character->mesh2D[12] = 0;
		character->mesh2D[13] = -ratio * character->yoffset;

		if (i == 0) {
			characterXoffset = 0;
			character->Translate(ratio * (x + characterXoffset), ratio * y);
		}
		else {
			character->Translate(ratio * (x + characterXoffset), ratio * y);
		}
		characterXoffset += character->xadvance * character->_scale.x;
		Batch::GetInstance()->Submit(character);
}
	va_end(argptr);
}

bool Doom::Gui::Button(std::string str, float x, float y,float scale, float width, float height, glm::vec4 btnColor, glm::vec4 pressedBtnColor, glm::vec4 textColor,Texture* texture)
{
	ApplyRelatedToPanelProperties(&x, &y);
	x += width * 0.5f;
	y -= height * 0.5f;
	float tempx = x;
	float tempy = y;
	float aRatio = Window::GetCamera().GetAspectRatio();
	this->btnColor = btnColor;
	bool tempResult = false;
	bool returnResult = false;
	glm::vec2 pos = glm::vec2(aRatio * x , aRatio * y);
	glm::dvec2 mousePos = ViewPort::GetInstance()->GetStaticMousePosition();

	double tempX = width * aRatio * 0.5f;
	double tempY = -height * aRatio * 0.5f;

	float vertecies[8] = {
		-tempX + pos.x, tempY + pos.y,
		 tempX + pos.x, tempY + pos.y,
		 tempX + pos.x,-tempY + pos.y,
		-tempX + pos.x,-tempY + pos.y,
	};


	if (Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		tempResult = (mousePos.x > vertecies[0] && mousePos.x < vertecies[2] && mousePos.y > vertecies[1] && mousePos.y < vertecies[7]);
		if (tempResult)
			this->btnColor = pressedBtnColor;
	}

	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1)) {
		returnResult = tempResult;
	}

	float ratio = aRatio;
	float _size = (HEIGHT * 2);
	Batch::GetInstance()->Submit(vertecies, this->btnColor,texture, glm::vec2(tempX / _size, tempY / _size), glm::vec2(pos.x / (_size / ratio), pos.y / _size) * (float)Window::GetCamera().GetZoomLevel(), (edgeRadius / (_size) * ratio));

	if (str.length() > 0) {
		xAlign = AlignHorizontally::XCENTER;
		yAlign = AlignVertically::YCENTER;
		Text(str, true, tempx - relatedPanelProperties.pos.x - relatedPanelProperties.margin.x, tempy - relatedPanelProperties.pos.y + relatedPanelProperties.yOffset + relatedPanelProperties.margin.y, scale, textColor);
		yAlign = AlignVertically::BOTTOM;
		xAlign = AlignHorizontally::LEFT;
	}
	else {
		lastTextProperties = UIProperties();
		relatedPanelProperties.yOffset += relatedPanelProperties.padding.y;
	}

	if (IsRelatedToPanel)
		relatedPanelProperties.yOffset += height - lastTextProperties.size.y;
	return returnResult;
}

void Doom::Gui::Panel(float x, float y, float width, float height, glm::vec4 color, bool changeColorWhenHovered,Texture* texture)
{

	if (IsRelatedToPanel) {
		relatedPanelProperties.pos = glm::vec2(x - width * 0.5f, y + height * 0.5f);
		relatedPanelProperties.size = glm::vec2(width, height);
	}

	float aRatio = Window::GetCamera().GetAspectRatio();
	glm::vec2 pos = glm::vec2(aRatio * x, aRatio * y);

	double tempX = width * aRatio * 0.5f;
	double tempY = -height * aRatio * 0.5f;

	currentPanelCoods[0] = -tempX + pos.x;  currentPanelCoods[1] =  tempY + pos.y;
	currentPanelCoods[2] =  tempX + pos.x;  currentPanelCoods[3] =  tempY + pos.y;
	currentPanelCoods[4] =  tempX + pos.x;  currentPanelCoods[5] = -tempY + pos.y;
	currentPanelCoods[6] = -tempX + pos.x;  currentPanelCoods[7] = -tempY + pos.y;

	float ratio = aRatio;
	float _size = (HEIGHT * 2);

	if (changeColorWhenHovered && IsPanelHovered()) {
		float alpha = color.a;
		color *= 0.9;
		color.a = alpha;
	}

	Batch::GetInstance()->Submit(currentPanelCoods, color, texture, glm::vec2(tempX / _size, tempY / _size), glm::vec2(pos.x / (_size / ratio), pos.y / _size) * (float)Window::GetCamera().GetZoomLevel(), (edgeRadius / (_size)* ratio));
}

void Doom::Gui::Bar(float x, float y, float value, float maxValue, glm::vec4 color,glm::vec4 outColor, float width, float height)
{
	ApplyRelatedToPanelProperties(&x, &y);
	x += width * 0.5f;
	y -= height * 0.5f;

	UIProperties temp = relatedPanelProperties;
	float tempCoords[8];
	for (size_t i = 0; i < 8; i++)
		tempCoords[i] = currentPanelCoods[i];
	Panel(x,y,width,height,outColor);
	float _width = width * (value / maxValue);
	float diff = width - _width;
	Panel(x - diff * 0.5f, y, _width, height, color);
	relatedPanelProperties = temp;
	for (size_t i = 0; i < 8; i++)
		currentPanelCoods[i] = tempCoords[i];
	relatedPanelProperties.yOffset += height + relatedPanelProperties.padding.y;
}

bool Doom::Gui::CheckBox(std::string label, bool * value, float x, float y, float size, glm::vec4 textColor, glm::vec4 imageColor)
{
	Texture* texture = nullptr;
	if (*value) {
		imageColor = COLORS::Green;
		texture = checkBoxTextureTrue;
	}
	else {
		imageColor = COLORS::Red;
		texture = checkBoxTextureFalse;
	}

	float tempradius = edgeRadius;
	edgeRadius = 0.0f;

	if (Button("", x, y, 24, size, size,imageColor, imageColor, COLORS::White, texture))
		*value = !*value;

	edgeRadius = tempradius;

	relatedPanelProperties.yOffset -= (size + relatedPanelProperties.padding.y);
	yAlign = AlignVertically::YCENTER;
	xAlign = AlignHorizontally::LEFT;
	Text(label, true, x + size, y - size * 0.5f, size * 0.5f, textColor);
	yAlign = AlignVertically::TOP;
	relatedPanelProperties.yOffset += (size - lastTextProperties.size.y);
	return *value;
}

float Doom::Gui::SliderFloat(std::string label, float * value, float min, float max, float x, float y, float width, float height,glm::vec4 sliderColor, glm::vec4 panelColor)
{
	glm::dvec2 mousePos = ViewPort::GetInstance()->GetStaticMousePosition();
	ApplyRelatedToPanelProperties(&x, &y);
	x += width * 0.5f;
	y -= height * 0.5f;
	UIProperties temp = relatedPanelProperties;
	float tempCoords[8];
	for (size_t i = 0; i < 8; i++)
		tempCoords[i] = currentPanelCoods[i];
	Panel(x, y, width, height, panelColor,true);

	if (*value < min || *value - min < 0.01 * max)
		*value = min;
	else if (*value > max || max - *value < 0.01 * max)
		*value = max;

	float _x = (width * *value) / max;

	if (Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		glm::vec2 mPos = ViewPort::GetInstance()->GetMousePositionToScreenSpace();
		if (IsPanelHovered()) {
			float _value = mPos.x - (x - width * 0.5f);
			*value = (_value * max) / width;
		}
	}

	Panel(x + _x - width * 0.5f, y, 0.05f * width, 1.5f * height, sliderColor);

	relatedPanelProperties = temp;

	yAlign = YCENTER;
	xAlign = XCENTER;
	Text("%f", true, width * 0.5f, -height * 0.5f, height * 0.5f, COLORS::White, 3, *value);
	relatedPanelProperties.yOffset -= (height * 0.5f + relatedPanelProperties.padding.y);
	xAlign = LEFT;
	Text(label, true, width + 10, -height * 0.5f, height * 0.5f, COLORS::White);
	yAlign = TOP;
	relatedPanelProperties.yOffset += (height - lastTextProperties.size.y);

	for (size_t i = 0; i < 8; i++)
		currentPanelCoods[i] = tempCoords[i];
	return *value;
}

void Doom::Gui::Image(float x, float y, float width, float height, Texture * texture, glm::vec4 color)
{
	ApplyRelatedToPanelProperties(&x, &y);

	float aRatio = Window::GetCamera().GetAspectRatio();
	glm::vec2 pos = glm::vec2(aRatio * x, aRatio * y);

	double tempX = width * aRatio * 0.5f;
	double tempY = -height * aRatio * 0.5f;

	float verteces[8];

	verteces[0] = -tempX + pos.x;  verteces[1] =  tempY + pos.y;
	verteces[2] =  tempX + pos.x;  verteces[3] =  tempY + pos.y;
	verteces[4] =  tempX + pos.x;  verteces[5] = -tempY + pos.y;
	verteces[6] = -tempX + pos.x;  verteces[7] = -tempY + pos.y;

	float ratio = aRatio;
	float _size = (HEIGHT * 2);

	Batch::GetInstance()->Submit(verteces, color, texture, glm::vec2(tempX / _size, tempY / _size), glm::vec2(pos.x / (_size / ratio), pos.y / _size) * (float)Window::GetCamera().GetZoomLevel(), (edgeRadius / (_size)* ratio));
	
	relatedPanelProperties.yOffset += height;
}

bool Doom::Gui::IsPanelHovered() const {
	glm::dvec2 mousePos = ViewPort::GetInstance()->GetStaticMousePosition();
	return (mousePos.x > currentPanelCoods[0] && mousePos.x < currentPanelCoods[2] && mousePos.y > currentPanelCoods[1] && mousePos.y < currentPanelCoods[7]);
}


void Doom::Gui::RelateToPanel()
{
	relatedPanelProperties.pos = glm::vec2(0.0f);
	relatedPanelProperties.size = glm::vec2(0.0f);
	relatedPanelProperties.margin = glm::vec2(0.0f);
	relatedPanelProperties.yOffset = 0.0f;
	IsRelatedToPanel = true;
}

void Doom::Gui::UnRelateToPanel()
{
	IsRelatedToPanel = false;
	relatedPanelProperties.pos = glm::vec2(0.0f);
	relatedPanelProperties.size = glm::vec2(0.0f);
	relatedPanelProperties.margin = glm::vec2(0.0f);
	relatedPanelProperties.yOffset = 0.0f;
}

void Doom::Gui::Begin() const
{
	Batch::GetInstance()->indexcount = 0;
	Batch::GetInstance()->Begin();
}

void Doom::Gui::End() const
{
	Batch::GetInstance()->End();
}

void Doom::Gui::LoadStandartFonts()
{
	
	Font* font = new Font();
	font->LoadFont("src/Fonts/calibri.txt", "src/Fonts/calibri.png");
	font->LoadCharacters();
	standartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/fonts/arial.txt", "src/fonts/arial.png");
	font->LoadCharacters();
	standartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/fonts/harrington.txt", "src/fonts/harrington.png");
	font->LoadCharacters();
	standartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/Fonts/Peak.txt", "src/Fonts/Peak.png");
	font->LoadCharacters();
	standartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/Fonts/playball.txt", "src/Fonts/playball.png");
	font->LoadCharacters();
	standartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/Fonts/segoe.txt", "src/Fonts/segoe.png");
	font->LoadCharacters();
	standartFonts.push_back(font);

	FontBind(standartFonts.front());
}

void Doom::Gui::ApplyRelatedToPanelProperties(float * x, float * y)
{
	*x += relatedPanelProperties.pos.x + relatedPanelProperties.margin.x;
	*y += relatedPanelProperties.pos.y - relatedPanelProperties.margin.y;
	if (relatedPanelProperties.autoAllignment)
		*y -= relatedPanelProperties.yOffset;
}

void Doom::Gui::RecalculateProjectionMatrix()
{
	float aspectRatio = Window::GetCamera().GetAspectRatio();
	ViewProjecTionRelatedToCamera = glm::ortho(-aspectRatio * (float)WIDTH, aspectRatio * (float)WIDTH, (float)-HEIGHT, (float)HEIGHT, -1.0f, 1.0f);
}

void Doom::Gui::ShutDown() {
	for (size_t i = 0; i < standartFonts.size(); i++)
	{
		delete standartFonts[i];
	}
	standartFonts.clear();
}
