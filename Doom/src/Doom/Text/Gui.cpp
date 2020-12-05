#include "Gui.h"
#include <functional>
#include "../Core/Timer.h"

using namespace Doom;

void Doom::Gui::Text(std::string str, int m_static, float x, float y, float startscale, glm::vec4 color, int charsAfterComma, ...)
{
	ApplyRelatedToPanelProperties(&x, &y);
	float scale = startscale / m_Font->m_Size;
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
				m_S = std::to_string(va_arg(argptr, int));
			else if (str[i + 1] == 'f') {
				m_S = std::to_string(va_arg(argptr, double));
			}
			else if (str[i + 1] == 's')
				m_S = (va_arg(argptr, std::string));
			size_t argSize = m_S.size();
			for (size_t j = 0; j < argSize; j++)
			{
				char pchar = m_S[j];
				if (pchar == '.' || pchar == ',') {
					dotPass = true;
					if (charsAfterComma == 0)
						break;
				}

				if(dotPass)
					counter1++;

				auto cIter = m_Font->m_Characters.find(pchar);
				if (cIter != m_Font->m_Characters.end()) {
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
			auto cIter = m_Font->m_Characters.find(strChar);
			if (cIter != m_Font->m_Characters.end()) {
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
		m_Character = characters[i];
		m_Character->m_Scale.x = scale;
		m_Character->m_Scale.y = scale;
		float tempHeight = m_Character->m_Height * m_Character->m_Scale.y;
		if (_height < tempHeight)
			_height = tempHeight;
		_width += m_Character->m_XAdvance * m_Character->m_Scale.x;

	}

	if (m_XAlign == AlignHorizontally::XCENTER) {
		x -= _width * 0.5;
	}
	if (m_YAlign == AlignVertically::YCENTER) {
		y += _height * 0.5;
	}

	m_LastTextProperties.m_Size = glm::vec2(_width, _height);
	m_LastTextProperties.m_Pos = glm::vec2(x, y);

	if (m_IsRelatedToPanel)
		m_RelatedPanelProperties.m_YOffset += _height + m_RelatedPanelProperties.m_Padding.y;

	unsigned int newLinesSize = newLines.size();
	for (unsigned int i = 0; i < size; i++)
	{
		for (unsigned int j = 0; j < newLinesSize; j++)
		{
			if (i == newLines[j]) {
				y -= startscale * 1.5;
				m_RelatedPanelProperties.m_YOffset += _height + m_RelatedPanelProperties.m_Padding.y;
				m_CharacterXOffset = 0;
				newLines.erase(newLines.begin() + j);
				break;
			}
		}
		m_Character = characters[i];
		m_Character->m_IsRelatedToCam = m_static;
		m_Character->m_Color = color;

		//Pos is top-left corner
		m_Character->m_Mesh2D[0] = 0;
		m_Character->m_Mesh2D[1] = -(ratio * (m_Character->m_Height + m_Character->m_YOffset));
		m_Character->m_Mesh2D[4] = (ratio * (m_Character->m_Width));
		m_Character->m_Mesh2D[5] = -(ratio * (m_Character->m_Height + m_Character->m_YOffset));
		m_Character->m_Mesh2D[8] = (ratio * (m_Character->m_Width));
		m_Character->m_Mesh2D[9] = -ratio * m_Character->m_YOffset;
		m_Character->m_Mesh2D[12] = 0;
		m_Character->m_Mesh2D[13] = -ratio * m_Character->m_YOffset;

		if (i == 0) {
			m_CharacterXOffset = 0;
			m_Character->Translate(ratio * (x + m_CharacterXOffset), ratio * y);
		}
		else {
			m_Character->Translate(ratio * (x + m_CharacterXOffset), ratio * y);
		}
		m_CharacterXOffset += m_Character->m_XAdvance * m_Character->m_Scale.x;
		Batch::GetInstance()->Submit(m_Character);
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
	this->m_BtnColor = btnColor;
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
			this->m_BtnColor = pressedBtnColor;
	}

	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1)) {
		returnResult = tempResult;
	}

	float ratio = aRatio;
	float _size = (g_Height * 2);
	Batch::GetInstance()->Submit(vertecies, this->m_BtnColor,texture, glm::vec2(tempX / _size, tempY / _size), glm::vec2(pos.x / (_size / ratio), pos.y / _size) * (float)Window::GetCamera().GetZoomLevel(), (m_EdgeRadius / (_size) * ratio));

	if (str.length() > 0) {
		m_XAlign = AlignHorizontally::XCENTER;
		m_YAlign = AlignVertically::YCENTER;
		Text(str, true, tempx - m_RelatedPanelProperties.m_Pos.x - m_RelatedPanelProperties.m_Margin.x, tempy - m_RelatedPanelProperties.m_Pos.y + m_RelatedPanelProperties.m_YOffset + m_RelatedPanelProperties.m_Margin.y, scale, textColor);
		m_YAlign = AlignVertically::BOTTOM;
		m_XAlign = AlignHorizontally::LEFT;
	}
	else {
		m_LastTextProperties = UIProperties();
		m_RelatedPanelProperties.m_YOffset += m_RelatedPanelProperties.m_Padding.y;
	}

	if (m_IsRelatedToPanel)
		m_RelatedPanelProperties.m_YOffset += height - m_LastTextProperties.m_Size.y;
	return returnResult;
}

void Doom::Gui::Panel(std::string label,float x, float y, float width, float height, glm::vec4 color, bool changeColorWhenHovered,Texture* texture)
{
	if (label != "") {
		auto iter = m_Panels.find(label);
		if (iter == m_Panels.end()) {
			m_Panels.insert(std::make_pair(label, Doom::Gui::PanelStruct()));
		}
	}

	auto iter = m_Panels.find(label);

	float aRatio = Window::GetCamera().GetAspectRatio();
	glm::vec2 pos = glm::vec2(aRatio * x, aRatio * y);

	double tempX = width * aRatio * 0.5f;
	double tempY = -height * aRatio * 0.5f;

	m_CurrentPanelCoods[0] = -tempX + pos.x;  m_CurrentPanelCoods[1] =  tempY + pos.y;
	m_CurrentPanelCoods[2] =  tempX + pos.x;  m_CurrentPanelCoods[3] =  tempY + pos.y;
	m_CurrentPanelCoods[4] =  tempX + pos.x;  m_CurrentPanelCoods[5] = -tempY + pos.y;
	m_CurrentPanelCoods[6] = -tempX + pos.x;  m_CurrentPanelCoods[7] = -tempY + pos.y;

	float ratio = aRatio;
	float _size = (g_Height * 2);

	bool isHovered = IsPanelHovered();

	iter->second.m_IsHovered = isHovered;
	if (isHovered) {
		iter->second.m_YScrollOffset -= Window::GetScrollYOffset() * 15;
			if (changeColorWhenHovered && !m_IsInteracting) {
				float alpha = color.a;
				color *= 0.9;
				color.a = alpha;
			}
	}

	if (m_IsRelatedToPanel) {
		m_RelatedPanelProperties.m_Pos = glm::vec2(x - width * 0.5f, iter->second.m_YScrollOffset + y + height * 0.5f);
		m_RelatedPanelProperties.m_Size = glm::vec2(width, height);
		m_RelatedPanelProperties.m_PanelPosForShader = glm::vec2(pos.x / (_size / ratio), pos.y / _size) * (float)Window::GetCamera().GetZoomLevel();
		m_RelatedPanelProperties.m_PanelSizeForShader = glm::vec2(tempX / _size, tempY / _size);
	}

	Batch::GetInstance()->Submit(m_CurrentPanelCoods, color, texture, m_RelatedPanelProperties.m_PanelSizeForShader, m_RelatedPanelProperties.m_PanelPosForShader, (m_EdgeRadius / (_size)* ratio));

	if (label.size() > 0) {
		glm::vec2 tempMargin = m_RelatedPanelProperties.m_Margin;
		glm::vec2 tempPadding = m_RelatedPanelProperties.m_Padding;
		m_RelatedPanelProperties.m_Margin = glm::vec2(5);
		m_RelatedPanelProperties.m_Padding = glm::vec2(0);
		m_XAlign = Gui::AlignHorizontally::LEFT;
		Text(label,true,m_EdgeRadius - m_RelatedPanelProperties.m_Margin.x);
		m_XAlign = Gui::AlignHorizontally::XCENTER;
		m_RelatedPanelProperties.m_Margin = tempMargin;
		m_RelatedPanelProperties.m_Padding = tempPadding;
	}
}

void Doom::Gui::Bar(float x, float y, float value, float maxValue, glm::vec4 color,glm::vec4 outColor, float width, float height)
{
	ApplyRelatedToPanelProperties(&x, &y);
	x += width * 0.5f;
	y -= height * 0.5f;

	UIProperties temp = m_RelatedPanelProperties;
	float tempCoords[8];
	for (size_t i = 0; i < 8; i++)
		tempCoords[i] = m_CurrentPanelCoods[i];
	Panel("",x,y,width,height,outColor);
	float _width = width * (value / maxValue);
	float diff = width - _width;
	Panel("",x - diff * 0.5f, y, _width, height, color);
	m_RelatedPanelProperties = temp;
	for (size_t i = 0; i < 8; i++)
		m_CurrentPanelCoods[i] = tempCoords[i];
	m_RelatedPanelProperties.m_YOffset += height + m_RelatedPanelProperties.m_Padding.y;
}

bool Doom::Gui::CheckBox(std::string label, bool * value, float x, float y, float size, glm::vec4 textColor, glm::vec4 imageColor)
{
	Texture* texture = nullptr;
	if (*value) {
		imageColor = COLORS::Green;
		texture = m_CheckBoxTextureTrue;
	}
	else {
		imageColor = COLORS::Red;
		texture = m_CheckBoxTextureFalse;
	}

	float tempradius = m_EdgeRadius;
	m_EdgeRadius = 0.0f;

	if (Button("", x, y, 24, size, size,imageColor, imageColor, COLORS::White, texture))
		*value = !*value;

	m_EdgeRadius = tempradius;

	m_RelatedPanelProperties.m_YOffset -= (size + m_RelatedPanelProperties.m_Padding.y);
	m_YAlign = AlignVertically::YCENTER;
	m_XAlign = AlignHorizontally::LEFT;
	Text(label, true, x + size, y - size * 0.5f, size, textColor);
	m_YAlign = AlignVertically::TOP;
	m_RelatedPanelProperties.m_YOffset += (size - m_LastTextProperties.m_Size.y);
	return *value;
}

float Doom::Gui::SliderFloat(std::string label, float * value, float min, float max, float x, float y, float width, float height,glm::vec4 sliderColor, glm::vec4 panelColor)
{
	auto iter = m_Interactable.find(label);
	if (iter == m_Interactable.end()) {
		m_Interactable.insert(std::make_pair(label, false));
		iter = m_Interactable.find(label);
	}

	glm::dvec2 mousePos = ViewPort::GetInstance()->GetStaticMousePosition();
	ApplyRelatedToPanelProperties(&x, &y);
	x += width * 0.5f;
	y -= height * 0.5f;
	UIProperties temp = m_RelatedPanelProperties;
	float tempCoords[8];
	for (size_t i = 0; i < 8; i++)
		tempCoords[i] = m_CurrentPanelCoods[i];
	Panel("",x, y, width, height, panelColor,true);

	float maxValue = 0;
	if (min < 0 && max < 0)
		maxValue = abs(min - max);
	else
		maxValue = max - min;

	float percent = ((*value - min)) / (max - min);
	float _x = percent * width;

	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1)) {
		if (IsPanelHovered()) {
			iter->second = true;
			m_IsInteracting = true;
		}
	}

	if (iter->second && Input::IsMouseReleased(Keycode::MOUSE_BUTTON_1)) {
		iter->second = !iter->second;
		m_IsInteracting = false;
	}

	if (iter->second) {
		glm::vec2 mPos = ViewPort::GetInstance()->GetMousePositionToScreenSpace();
			float _value = (mPos.x - x - width * 0.5f);
			*value = ((1 + ((_value) / (width))) * maxValue) + min;
			if (*value > max)
				*value = max;
			else if (*value < min)
				*value = min;
	}

	Panel("",x + _x - width * 0.5f, y, 0.05f * width, 1.5f * height, sliderColor);

	m_RelatedPanelProperties = temp;

	m_YAlign = YCENTER;
	m_XAlign = XCENTER;
	Text("%f", true, width * 0.5f, -height * 0.5f, height * 0.5f, COLORS::White, 3, *value);
	m_RelatedPanelProperties.m_YOffset -= (height * 0.5f + m_RelatedPanelProperties.m_Padding.y);
	m_XAlign = LEFT;
	Text(label, true, width + 10, -height * 0.5f, height * 0.5f, COLORS::White);
	m_YAlign = TOP;
	m_RelatedPanelProperties.m_YOffset += (height - m_LastTextProperties.m_Size.y);

	for (size_t i = 0; i < 8; i++)
		m_CurrentPanelCoods[i] = tempCoords[i];
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
	float _size = (g_Height * 2);

	Batch::GetInstance()->Submit(verteces, color, texture, glm::vec2(tempX / _size, tempY / _size), glm::vec2(pos.x / (_size / ratio), pos.y / _size) * (float)Window::GetCamera().GetZoomLevel(), (m_EdgeRadius / (_size)* ratio));
	
	m_RelatedPanelProperties.m_YOffset += height + m_RelatedPanelProperties.m_Padding.y;
}

bool Doom::Gui::CollapsingHeader(std::string label, float x, float y,glm::vec4 color)
{
	bool hovered = false;
	bool pressed = false;
	float height = 25.f;
	float width = m_RelatedPanelProperties.m_Size.x;
	ApplyRelatedToPanelProperties(&x, &y);
	x += m_RelatedPanelProperties.m_Size.x * 0.5f - m_RelatedPanelProperties.m_Margin.x;
	y -= height * 0.5f;
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
	float _size = (g_Height * 2);

	if (m_IsInteracting == false) {
		glm::dvec2 mousePos = ViewPort::GetInstance()->GetStaticMousePosition();
		hovered = (mousePos.x > verteces[0] && mousePos.x < verteces[2] && mousePos.y > verteces[1] && mousePos.y < verteces[7]);
		if (hovered) {
			color[0] *= 0.7f; color[1] *= 0.7f; color[2] *= 0.7f;
		}

		if (hovered && Input::IsMousePressed(Keycode::MOUSE_BUTTON_1)) {
			pressed = true;
		}
	}

	Batch::GetInstance()->Submit(verteces, color, nullptr, glm::vec2(tempX / _size, tempY / _size), glm::vec2(pos.x / (_size / ratio), pos.y / _size) * (float)Window::GetCamera().GetZoomLevel(), (m_EdgeRadius / (_size)* ratio));

	auto iter = m_Interactable.find(label);
	if (iter != m_Interactable.end()) {
		if (pressed) {
			iter->second = !iter->second;
		}
	}
	else {
		m_Interactable.insert(std::make_pair(label, pressed));
	}

	bool flag = m_Interactable.find(label)->second;

	if(flag)
		Image(7.5, -height * 0.5f, 15, 15, m_TriangleDownTexture);
	else
		Image(7.5, -height * 0.5f, 15, 15, m_TriangleRightTexture);

	m_RelatedPanelProperties.m_YOffset -= (height * 0.5f + m_RelatedPanelProperties.m_Padding.y);

	Text(label, true, 7.5 + 15 + 5, 0, 20);

	m_RelatedPanelProperties.m_YOffset += height - m_LastTextProperties.m_Size.y;

	return flag;
}

bool Doom::Gui::IsPanelHovered() {
	glm::dvec2 mousePos = ViewPort::GetInstance()->GetStaticMousePosition();
	bool flag = (mousePos.x > m_CurrentPanelCoods[0] && mousePos.x < m_CurrentPanelCoods[2] && mousePos.y > m_CurrentPanelCoods[1] && mousePos.y < m_CurrentPanelCoods[7]);
	if (flag)
		m_IsAnyPanelHovered = flag;
	return flag;
}

void Doom::Gui::RelateToPanel()
{
	m_RelatedPanelProperties.m_Pos = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_Size = glm::vec2(0.0f);
	//relatedPanelProperties.margin = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_YOffset = 0.0f;
	m_IsRelatedToPanel = true;
}

void Doom::Gui::UnRelateToPanel()
{
	m_IsRelatedToPanel = false;
	m_RelatedPanelProperties.m_Pos = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_Size = glm::vec2(0.0f);
	//relatedPanelProperties.margin = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_YOffset = 0.0f;
}

void Doom::Gui::Begin() const
{
	Batch::GetInstance()->m_TIndexCount = 0;
	Batch::GetInstance()->BeginText();
}

void Doom::Gui::End() const
{
	Batch::GetInstance()->EndText();
}

inline void Doom::Gui::FontBind(Font * font)
{
	this->m_Font = font;
}

void Doom::Gui::LoadStandartFonts()
{
	
	Font* font = new Font();
	font->LoadFont("src/Fonts/calibri.txt", "src/Fonts/calibri.png");
	font->LoadCharacters();
	m_StandartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/fonts/arial.txt", "src/fonts/arial.png");
	font->LoadCharacters();
	m_StandartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/fonts/harrington.txt", "src/fonts/harrington.png");
	font->LoadCharacters();
	m_StandartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/Fonts/Peak.txt", "src/Fonts/Peak.png");
	font->LoadCharacters();
	m_StandartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/Fonts/playball.txt", "src/Fonts/playball.png");
	font->LoadCharacters();
	m_StandartFonts.push_back(font);

	font = new Font();
	font->LoadFont("src/Fonts/segoe.txt", "src/Fonts/segoe.png");
	font->LoadCharacters();
	m_StandartFonts.push_back(font);

	FontBind(m_StandartFonts.front());
}

void Doom::Gui::ApplyRelatedToPanelProperties(float * x, float * y)
{
	*x += m_RelatedPanelProperties.m_Pos.x + m_RelatedPanelProperties.m_Margin.x;
	*y += m_RelatedPanelProperties.m_Pos.y - m_RelatedPanelProperties.m_Margin.y;
	if (m_RelatedPanelProperties.m_AutoAllignment)
		*y -= m_RelatedPanelProperties.m_YOffset;
}

void Doom::Gui::RecalculateProjectionMatrix()
{
	float aspectRatio = Window::GetCamera().GetAspectRatio();
	m_ViewProjecTionMat4RelatedToCamera = glm::ortho(-aspectRatio * (float)g_Width, aspectRatio * (float)g_Width, (float)-g_Height, (float)g_Height, -1.0f, 1.0f);
}

void Doom::Gui::ShutDown() {
	for (size_t i = 0; i < m_StandartFonts.size(); i++)
	{
		delete m_StandartFonts[i];
	}
	m_StandartFonts.clear();
}
