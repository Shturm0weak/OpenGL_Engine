#include "pch.h"
#include "Gui.h"
#include <functional>
#include "iomanip"
#include "sstream"

using namespace Doom;

Gui::Gui()
{
	RecalculateProjectionMatrix();

	Texture::AsyncGet([=](Texture* t) {
		m_CheckBoxTextureTrue = t;
		}, "src/UIimages/CheckMarkTrue.png");

	Texture::AsyncGet([=](Texture* t) {
		m_CheckBoxTextureFalse = t;
		}, "src/UIimages/CheckMarkFalse.png");

	Texture::AsyncGet([=](Texture* t) {
		m_TriangleRightTexture = t;
		}, "src/UIimages/TriangleRight.png");

	Texture::AsyncGet([=](Texture* t) {
		m_TriangleDownTexture = t;
		}, "src/UIimages/TriangleDown.png");
}

void Gui::Text(const std::wstring& str, int relatedToCamera, float x, float y, float startscale, glm::vec4 color, int charsAfterComma, ...)
{
	ApplyRelatedToPanelProperties(&x, &y);
	float scale = startscale / m_Font->m_Size;
	float ratio = Window::GetInstance().GetCamera().m_Ratio;
	std::vector<Character*> characters;
	std::vector<size_t> newLines;
	size_t counter = 0;
	bool isDotPass = false;
	va_list argptr;
	va_start(argptr, charsAfterComma);
	size_t strSize = str.size();
	for (size_t i = 0; i < strSize; i++)
	{
		wchar_t strChar = str[i];
		//Not sure about this, maybe it is better to refactor
		if (strChar == L'#')
		{
			if (str[i + 1] == L'%')
				FindCharInFont(characters, str[i + 1]);
			else if (str[i + 1] == L'#')
			{
				while (str[i + 1] != L' ' && i + 1 != strSize - 1)
				{
					i++;
				}
			}
			i++;
		}
		else if (strChar == L'%') 
		{
			if (str[i + 1] == L'd')
				m_S = std::to_string(va_arg(argptr, int64_t));
			else if (str[i + 1] == 'f')
				m_S = std::to_string(va_arg(argptr, double));
			else if (str[i + 1] == 's')
				m_S = (va_arg(argptr, std::string));
			size_t argSize = m_S.size();
			for (size_t j = 0; j < argSize; j++)
			{
				wchar_t pchar = m_S[j];
				if (pchar == L'.' || pchar == L',')
				{
					isDotPass = true;
					if (charsAfterComma == 0)
						break;
				}

				if(isDotPass)
					counter++;

				FindCharInFont(characters, pchar);

				if (counter > charsAfterComma)
					break;
			}
			isDotPass = false;
			counter = 0;
			i++;
		}
		else if (strChar == L'\n')
		{
			newLines.push_back(characters.size());
			continue;
		}
		else
			FindCharInFont(characters, strChar);
	}

	size_t size = characters.size();

	glm::vec2 rowSize = glm::vec2(0.0f);

	for (size_t i = 0; i < size; i++)
	{
		m_Character = characters[i];
		m_Character->m_Scale.x = scale;
		m_Character->m_Scale.y = scale;
		float tempHeight = m_Character->m_Height * m_Character->m_Scale.y;
		if (rowSize.y < tempHeight)
			rowSize.y = tempHeight;
		rowSize.x += m_Character->m_XAdvance * m_Character->m_Scale.x;

	}

	if (m_XAlign == AlignHorizontally::XCENTER)
		x -= rowSize.x * 0.5;
	if (m_YAlign == AlignVertically::YCENTER)
		y += rowSize.y * 0.5;

	m_LastTextProperties.m_Size = glm::vec2(rowSize.x, rowSize.y);
	m_LastTextProperties.m_Pos = glm::vec2(x, y);

	if (m_IsRelatedToPanel)
		m_RelatedPanelProperties.m_YOffset += rowSize.y + m_RelatedPanelProperties.m_Padding.y;

	size_t newLinesSize = newLines.size();
	for (size_t i = 0; i < size; i++)
	{
		for (size_t j = 0; j < newLinesSize; j++)
		{
			if (i == newLines[j]) 
			{
				y -= startscale * 1.5;
				m_RelatedPanelProperties.m_YOffset += rowSize.y + m_RelatedPanelProperties.m_Padding.y;
				m_CharacterXOffset = 0;
				newLines.erase(newLines.begin() + j);
				break;
			}
		}
		m_Character = characters[i];
		m_Character->m_IsRelatedToCam = relatedToCamera;
		m_Character->m_Color = color;

		//Pos is top-left corner
		m_Character->m_Mesh2D[0] = 0;
		m_Character->m_Mesh2D[1] = -(m_Character->m_Height + m_Character->m_YOffset);
		m_Character->m_Mesh2D[4] =  (m_Character->m_Width);
		m_Character->m_Mesh2D[5] = -(m_Character->m_Height + m_Character->m_YOffset);
		m_Character->m_Mesh2D[8] =  (m_Character->m_Width);
		m_Character->m_Mesh2D[9] =  -m_Character->m_YOffset;
		m_Character->m_Mesh2D[12] = 0;
		m_Character->m_Mesh2D[13] = -m_Character->m_YOffset;

		if (i == 0) m_CharacterXOffset = 0;
		m_Character->m_Position.x = x + m_CharacterXOffset;
		m_Character->m_Position.y = y;
		m_CharacterXOffset += m_Character->m_XAdvance * m_Character->m_Scale.x;

		Batch::GetInstance().Submit(m_Character);
	}
	va_end(argptr);
}

bool Gui::Button(const std::wstring& str, float x, float y,float scale, float width, float height, glm::vec4 btnColor, glm::vec4 pressedBtnColor, glm::vec4 textColor,Texture* texture)
{
	ApplyRelatedToPanelProperties(&x, &y);
	x += width * 0.5f;
	y -= height * 0.5f;
	if (m_XAlign == AlignHorizontally::XCENTER)
		x += m_RelatedPanelProperties.m_Size.x * 0.5f - m_RelatedPanelProperties.m_Margin.x - width * 0.5f;
	if (m_YAlign == AlignVertically::YCENTER)
		y -= m_RelatedPanelProperties.m_Size.y * 0.5f - m_RelatedPanelProperties.m_Margin.y - height * 0.5f;
	float ratio = Window::GetInstance().GetCamera().m_Ratio;
	this->m_BtnColor = btnColor;
	bool tempResult = false;
	bool returnResult = false;
	glm::dvec2 mousePos = ViewPort::GetInstance().GetStaticMousePosition();
	glm::vec2 pos = glm::vec2(x, y);
	glm::vec2 realSize = { width * 0.5f, -height * 0.5f };

	float vertecies[8] = {
		-realSize.x + pos.x, realSize.y + pos.y,
		 realSize.x + pos.x, realSize.y + pos.y,
		 realSize.x + pos.x,-realSize.y + pos.y,
		-realSize.x + pos.x,-realSize.y + pos.y,
	};

	if (Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) 
	{
		tempResult = (mousePos.x > vertecies[0] && mousePos.x < vertecies[2] && mousePos.y > vertecies[1] && mousePos.y < vertecies[7]);
		if (tempResult)
			this->m_BtnColor = pressedBtnColor;
	}

	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
		returnResult = tempResult;

	float size = (g_ScaleUI * 2);
	Batch::GetInstance().Submit(vertecies, this->m_BtnColor, texture);

	if (str.length() > 0) 
	{
		m_XAlign = AlignHorizontally::XCENTER;
		m_YAlign = AlignVertically::YCENTER;
		if (m_RelatedPanelProperties.m_AutoAllignment)
			y += m_RelatedPanelProperties.m_YOffset;
		Text(str, true, x - m_RelatedPanelProperties.m_Pos.x - m_RelatedPanelProperties.m_Margin.x, y - m_RelatedPanelProperties.m_Pos.y + m_RelatedPanelProperties.m_Margin.y, scale, textColor); //Maybe it will be needed  + m_RelatedPanelProperties.m_Margin.y
		m_YAlign = AlignVertically::BOTTOM;
		m_XAlign = AlignHorizontally::LEFT;
	}
	else
	{
		m_LastTextProperties = UIProperties();
		m_RelatedPanelProperties.m_YOffset += m_RelatedPanelProperties.m_Padding.y;
	}

	if (m_IsRelatedToPanel)
		m_RelatedPanelProperties.m_YOffset += height - m_LastTextProperties.m_Size.y;
	return returnResult;
}

void Gui::Panel(const std::wstring& label,float x, float y, float width, float height, glm::vec4 color, bool changeColorWhenHovered,Texture* texture)
{
	if (label != L"") 
	{
		auto iter = m_Panels.find(label);
		if (iter == m_Panels.end())
			m_Panels.insert(std::make_pair(label, Gui::PanelStruct()));
	}

	auto iter = m_Panels.find(label);
	Camera& camera = Window::GetInstance().GetCamera();
	float ratio = camera.m_Ratio;
	glm::vec2 pos = glm::vec2(x, y);
	glm::vec2 realSize = { width * 0.5f, -height * 0.5f };

	m_CurrentPanelCoods[0] = -realSize.x + pos.x;  m_CurrentPanelCoods[1] =  realSize.y + pos.y;
	m_CurrentPanelCoods[2] =  realSize.x + pos.x;  m_CurrentPanelCoods[3] =  realSize.y + pos.y;
	m_CurrentPanelCoods[4] =  realSize.x + pos.x;  m_CurrentPanelCoods[5] = -realSize.y + pos.y;
	m_CurrentPanelCoods[6] = -realSize.x + pos.x;  m_CurrentPanelCoods[7] = -realSize.y + pos.y;

	float size = (g_ScaleUI * 2);

	bool isHovered = IsPanelHovered();

	iter->second.m_IsHovered = isHovered;
	if (isHovered) 
	{
		if (changeColorWhenHovered && !m_IsInteracting) 
		{
			float alpha = color.a;
			color *= 0.9;
			color.a = alpha;
		}
	}

	if (m_IsRelatedToPanel)
	{
		m_RelatedPanelProperties.m_Pos = glm::vec2(x - width * 0.5f, y + height * 0.5f);
		m_RelatedPanelProperties.m_Size = glm::vec2(width, height);
	}

	Batch::GetInstance().Submit(m_CurrentPanelCoods, color, texture);

	if (label.size() > 0 && label.find(L"##") == std::string::npos)
	{
		glm::vec2 tempMargin = m_RelatedPanelProperties.m_Margin;
		glm::vec2 tempPadding = m_RelatedPanelProperties.m_Padding;
		m_RelatedPanelProperties.m_Margin = glm::vec3(5);
		m_RelatedPanelProperties.m_Padding = glm::vec2(0);
		//m_XAlign = Gui::AlignHorizontally::LEFT;
		x = 0;
		if (m_XAlign == AlignHorizontally::XCENTER)
			x = m_RelatedPanelProperties.m_Size.x * 0.5f - m_RelatedPanelProperties.m_Margin.x;
		
		Text(label, true, x, 0, m_RelatedPanelProperties.m_PanelLabelSize);
		//m_XAlign = Gui::AlignHorizontally::XCENTER;
		m_RelatedPanelProperties.m_Margin = tempMargin;
		m_RelatedPanelProperties.m_Padding = tempPadding;
	}
}

void Gui::Bar(float x, float y, float value, float maxValue, glm::vec4 color,glm::vec4 outColor, float width, float height)
{
	ApplyRelatedToPanelProperties(&x, &y);
	x += width * 0.5f;
	y -= height * 0.5f;
	std::wstring uniqueId = std::to_wstring(x);
	UIProperties temp = m_RelatedPanelProperties;
	float tempCoords[8];
	for (size_t i = 0; i < 8; i++)
		tempCoords[i] = m_CurrentPanelCoods[i];
	Panel(L"##Bar1" + uniqueId, x, y, width, height, outColor);
	float currentWidth = width * (value / maxValue);
	float diff = width - currentWidth;
	Panel(L"##Bar2" + uniqueId, x - diff * 0.5f, y, currentWidth, height, color);
	m_RelatedPanelProperties = temp;
	for (size_t i = 0; i < 8; i++)
		m_CurrentPanelCoods[i] = tempCoords[i];
	m_RelatedPanelProperties.m_YOffset += height + m_RelatedPanelProperties.m_Padding.y;
}

bool Gui::CheckBox(const std::wstring& label, bool * value, float x, float y, float size, glm::vec4 textColor, glm::vec4 imageColor)
{
	Texture* texture = nullptr;
	bool pressed = false;
	if (*value) 
	{
		imageColor = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		texture = m_CheckBoxTextureTrue;
	}
	else 
	{
		imageColor = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		texture = m_CheckBoxTextureFalse;
	}

	if (Button(L"##Button" + std::to_wstring(x), x, y, size, size, size, imageColor, imageColor, glm::vec4(1.0f), texture))
	{
		*value = !*value;
		pressed = true;
	}

	m_RelatedPanelProperties.m_YOffset -= (size + m_RelatedPanelProperties.m_Padding.y);
	m_YAlign = AlignVertically::YCENTER;
	m_XAlign = AlignHorizontally::LEFT;
	Text(label, true, x + size, y - size * 0.5f, size, textColor);
	m_YAlign = AlignVertically::TOP;
	m_RelatedPanelProperties.m_YOffset += (size - m_LastTextProperties.m_Size.y);
	return pressed;
}

bool Gui::SliderFloat(const std::wstring& label, float * value, float min, float max, float x, float y, float width, float height,glm::vec4 sliderColor, glm::vec4 panelColor)
{
	auto iter = m_Interactable.find(label);
	if (iter == m_Interactable.end()) {
		m_Interactable.insert(std::make_pair(label, false));
		iter = m_Interactable.find(label);
	}

	std::wstring uniqueId = std::to_wstring(x);

	glm::dvec2 mousePos = ViewPort::GetInstance().GetStaticMousePosition();
	ApplyRelatedToPanelProperties(&x, &y);
	x += width * 0.5f;
	y -= height * 0.5f;
	UIProperties temp = m_RelatedPanelProperties;
	float tempCoords[8];
	for (size_t i = 0; i < 8; i++)
		tempCoords[i] = m_CurrentPanelCoods[i];
	Panel(L"##OuterPanel" + uniqueId, x, y, width, height, panelColor, true);

	float maxValue = 0;
	if (min < 0 && max < 0) maxValue = abs(min - max);
	else maxValue = max - min;

	float percent = ((*value - min)) / (max - min);
	float xPos = percent * width;

	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
	{
		if (IsPanelHovered()) 
		{
			iter->second = true;
			m_IsInteracting = true;
		}
	}

	if (iter->second && Input::IsMouseReleased(Keycode::MOUSE_BUTTON_1))
	{
		iter->second = !iter->second;
		m_IsInteracting = false;
	}

	if (iter->second) 
	{
		glm::vec2 mPos = ViewPort::GetInstance().GetStaticMousePosition();
		float _value = (mPos.x - x - width * 0.5f);
		*value = ((1 + ((_value) / (width))) * maxValue) + min;
		if (*value > max) *value = max;
		else if (*value < min) *value = min;
	}

	Panel(L"##InnerPanel" + uniqueId, x + xPos - width * 0.5f, y, 0.05f * width, 1.5f * height, sliderColor);

	m_RelatedPanelProperties = temp;

	m_YAlign = YCENTER;
	m_XAlign = XCENTER;
	Text(L"##SliderFloat" + uniqueId + L" %f", true, width * 0.5f, -height * 0.5f, height * 0.5f, glm::vec4(1.0f), 3, *value);
	m_RelatedPanelProperties.m_YOffset -= (height * 0.5f + m_RelatedPanelProperties.m_Padding.y);
	m_XAlign = LEFT;
	Text(label, true, width + 10, -height * 0.5f, height * 0.5f, glm::vec4(1.0f));
	m_YAlign = TOP;
	m_RelatedPanelProperties.m_YOffset += (height - m_LastTextProperties.m_Size.y);

	for (size_t i = 0; i < 8; i++)
		m_CurrentPanelCoods[i] = tempCoords[i];
	return m_IsInteracting;
}

void Gui::InputInt(const std::wstring& label, int64_t* value, float x, float y, float width, float height, glm::vec4 panelColor)
{
	auto iter = m_Interactable.find(label);
	if (iter == m_Interactable.end()) {
		m_Interactable.insert(std::make_pair(label, false));
		iter = m_Interactable.find(label);
	}

	auto iter1 = m_NegativeInputs.find(label);
	if (iter1 == m_NegativeInputs.end()) {
		m_NegativeInputs.insert(std::make_pair(label, false));
		iter1 = m_NegativeInputs.find(label);
	}

	glm::dvec2 mousePos = ViewPort::GetInstance().GetStaticMousePosition();
	ApplyRelatedToPanelProperties(&x, &y);

	x += width * 0.5f;
	y -= height * 0.5f;
	std::wstring uniqueId = std::to_wstring(x);
	UIProperties temp = m_RelatedPanelProperties;
	float tempCoords[8];
	for (size_t i = 0; i < 8; i++)
		tempCoords[i] = m_CurrentPanelCoods[i];

	Panel(L"##InputInt" + uniqueId, x, y, width, height, panelColor, true);

	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
	{
		if (IsPanelHovered())
		{
			iter->second = true;
			m_IsInteracting = true;
		}
		else 
		{
			iter->second = false;
			m_IsInteracting = false;
		}
	}

	m_RelatedPanelProperties = temp;

	if (iter->second == true)
	{
		std::string stringValue;
		std::ostringstream Q;
		Q << std::setprecision(6) << std::setw(20) << *value;
		stringValue = Q.str();

		int key = -1;

		if (Input::IsKeyPressed(Keycode::KEY_0)) key = Keycode::KEY_0;
		else if (Input::IsKeyPressed(Keycode::KEY_1)) key = Keycode::KEY_1;
		else if (Input::IsKeyPressed(Keycode::KEY_2)) key = Keycode::KEY_2;
		else if (Input::IsKeyPressed(Keycode::KEY_3)) key = Keycode::KEY_3;
		else if (Input::IsKeyPressed(Keycode::KEY_4)) key = Keycode::KEY_4;
		else if (Input::IsKeyPressed(Keycode::KEY_5)) key = Keycode::KEY_5;
		else if (Input::IsKeyPressed(Keycode::KEY_6)) key = Keycode::KEY_6;
		else if (Input::IsKeyPressed(Keycode::KEY_7)) key = Keycode::KEY_7;
		else if (Input::IsKeyPressed(Keycode::KEY_8)) key = Keycode::KEY_8;
		else if (Input::IsKeyPressed(Keycode::KEY_9)) key = Keycode::KEY_9;
		
		if (key != -1)
		{
			stringValue += std::to_string(key - 48);
			std::istringstream(stringValue) >> *value;
		}

		//if (Input::IsKeyPressed(Keycode::MINUS))
		//{
		//	stringValue = "-0";
		//	std::istringstream(stringValue) >> *value;
		//	iter1->second = !iter1->second;
		//}

		if (Input::IsKeyPressed(Keycode::KEY_BACKSPACE))
		{
			if (stringValue.empty() == false) stringValue.pop_back();
			if (*stringValue.rbegin() == 32) *value = 0;
			else std::istringstream(stringValue) >> *value;
		}

		if (iter1->second) *value *= *value < 0 ? 1 : -1;
	}

	m_YAlign = YCENTER;
	m_XAlign = XCENTER;
	Text(L"%d", true, width * 0.5f, -height * 0.5f, height * 0.5f, glm::vec4(1.0f), 6, *value);
	m_RelatedPanelProperties.m_YOffset -= (height * 0.5f + m_RelatedPanelProperties.m_Padding.y);
	m_XAlign = LEFT;
	Text(label, true, width + 10, -height * 0.5f, height * 0.5f, glm::vec4(1.0f));
	m_YAlign = TOP;
	m_RelatedPanelProperties.m_YOffset += (height - m_LastTextProperties.m_Size.y);

	for (size_t i = 0; i < 8; i++)
		m_CurrentPanelCoods[i] = tempCoords[i];
}

void Gui::InputDouble(const std::wstring& label, double* value, float x, float y, float width, float height, glm::vec4 panelColor)
{
	auto iter = m_Interactable.find(label);
	if (iter == m_Interactable.end()) {
		m_Interactable.insert(std::make_pair(label, false));
		iter = m_Interactable.find(label);
	}

	auto iter1 = m_NegativeInputs.find(label);
	if (iter1 == m_NegativeInputs.end()) {
		m_NegativeInputs.insert(std::make_pair(label, false));
		iter1 = m_NegativeInputs.find(label);
	}

	glm::dvec2 mousePos = ViewPort::GetInstance().GetStaticMousePosition();
	ApplyRelatedToPanelProperties(&x, &y);
	x += width * 0.5f;
	y -= height * 0.5f;
	std::wstring uniqueId = std::to_wstring(x);
	UIProperties temp = m_RelatedPanelProperties;
	float tempCoords[8];
	for (size_t i = 0; i < 8; i++)
		tempCoords[i] = m_CurrentPanelCoods[i];
	Panel(L"##InputDouble" + uniqueId, x, y, width, height, panelColor, true);

	if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
	{
		if (IsPanelHovered())
		{
			iter->second = true;
			m_IsInteracting = true;
		}
		else
		{
			iter->second = false;
			m_IsInteracting = false;
		}
	}

	m_RelatedPanelProperties = temp;

	std::ostringstream Q;
	Q << std::setprecision(6) << std::setw(20) << *value;

	auto iter2 = m_DoubleInput.find(label);
	if (iter2 == m_DoubleInput.end()) {
		m_DoubleInput.insert(std::make_pair(label, Q.str()));
		iter2 = m_DoubleInput.find(label);
	}

	iter2->second.erase(std::remove(iter2->second.begin(), iter2->second.end(), 32), iter2->second.end());
	std::string stringValue = iter2->second;

	size_t index = stringValue.find('.');

	if (iter->second == true)
	{
		int key = -1;

		if (Input::IsKeyPressed(Keycode::KEY_0)) key = Keycode::KEY_0;
		else if (Input::IsKeyPressed(Keycode::KEY_1)) key = Keycode::KEY_1;
		else if (Input::IsKeyPressed(Keycode::KEY_2)) key = Keycode::KEY_2;
		else if (Input::IsKeyPressed(Keycode::KEY_3)) key = Keycode::KEY_3;
		else if (Input::IsKeyPressed(Keycode::KEY_4)) key = Keycode::KEY_4;
		else if (Input::IsKeyPressed(Keycode::KEY_5)) key = Keycode::KEY_5;
		else if (Input::IsKeyPressed(Keycode::KEY_6)) key = Keycode::KEY_6;
		else if (Input::IsKeyPressed(Keycode::KEY_7)) key = Keycode::KEY_7;
		else if (Input::IsKeyPressed(Keycode::KEY_8)) key = Keycode::KEY_8;
		else if (Input::IsKeyPressed(Keycode::KEY_9)) key = Keycode::KEY_9;

		if (key != -1)
		{
			if (stringValue.size() == 1 && stringValue[0] == '0') stringValue.pop_back();
			if (stringValue.size() < 5) 
			{
				stringValue += std::to_string(key - 48);
				std::istringstream(stringValue) >> *value;
			}
		}

		//if (Input::IsKeyPressed(Keycode::MINUS))
		//{
		//	stringValue = "-0";
		//	std::istringstream(stringValue) >> *value;
		//	iter1->second = !iter1->second;
		//}
		if (Input::IsKeyPressed(Keycode::PERIOD))
		{
			if (index == std::string::npos)
			{
				stringValue += ".";
				std::istringstream(stringValue) >> *value;
			}
		}
		else if (Input::IsKeyPressed(Keycode::KEY_BACKSPACE))
		{
			if (stringValue.empty() == false) stringValue.pop_back();
			if (stringValue.empty() == true) stringValue += '0';
			std::istringstream(stringValue) >> *value;
		}

		if (iter1->second) *value *= *value < 0 ? 1 : -1;
		iter2->second = stringValue;
	}

	m_YAlign = YCENTER;
	m_XAlign = XCENTER;
	int charsAfterComma = index == std::string::npos ? 0 : stringValue.size() - index;
	Text(L"%s", true, width * 0.5f, -height * 0.5f, height * 0.5f, glm::vec4(1.0f), charsAfterComma, stringValue);
	m_RelatedPanelProperties.m_YOffset -= (height * 0.5f + m_RelatedPanelProperties.m_Padding.y);
	m_XAlign = LEFT;
	Text(label, true, width + 10, -height * 0.5f, height * 0.5f, glm::vec4(1.0f));
	m_YAlign = TOP;
	m_RelatedPanelProperties.m_YOffset += (height - m_LastTextProperties.m_Size.y);

	for (size_t i = 0; i < 8; i++)
		m_CurrentPanelCoods[i] = tempCoords[i];
}

void Gui::Image(float x, float y, float width, float height, Texture * texture, glm::vec4 color)
{
	ApplyRelatedToPanelProperties(&x, &y); //TODO: Need somehow to fix it, kind of a flag if a panel is existed
	glm::vec2 pos = glm::vec2(x, y);
	glm::vec2 realSize = { width * 0.5f, -height * 0.5f };

	float verteces[8];

	verteces[0] = -realSize.x + pos.x;  verteces[1] =  realSize.y + pos.y;
	verteces[2] =  realSize.x + pos.x;  verteces[3] =  realSize.y + pos.y;
	verteces[4] =  realSize.x + pos.x;  verteces[5] = -realSize.y + pos.y;
	verteces[6] = -realSize.x + pos.x;  verteces[7] = -realSize.y + pos.y;

	Batch::GetInstance().Submit(verteces, color, texture);
	
	m_RelatedPanelProperties.m_YOffset += height + m_RelatedPanelProperties.m_Padding.y;
}

bool Gui::CollapsingHeader(const std::wstring& label, float x, float y, float height, glm::vec4 color)
{
	bool hovered = false;
	bool pressed = false;
	float width = m_RelatedPanelProperties.m_Size.x;
	ApplyRelatedToPanelProperties(&x, &y);
	x += m_RelatedPanelProperties.m_Size.x * 0.5f - m_RelatedPanelProperties.m_Margin.x;
	y -= height * 0.5f;
	glm::vec2 pos = glm::vec2(x, y);
	glm::vec2 realSize = { width * 0.5f, -height * 0.5f };

	float verteces[8];

	verteces[0] = -realSize.x + pos.x;  verteces[1] =  realSize.y + pos.y;
	verteces[2] =  realSize.x + pos.x;  verteces[3] =  realSize.y + pos.y;
	verteces[4] =  realSize.x + pos.x;  verteces[5] = -realSize.y + pos.y;
	verteces[6] = -realSize.x + pos.x;  verteces[7] = -realSize.y + pos.y;

	if (m_IsInteracting == false) 
	{
		glm::dvec2 mousePos = ViewPort::GetInstance().GetStaticMousePosition();
		hovered = (mousePos.x > verteces[0] && mousePos.x < verteces[2] && mousePos.y > verteces[1] && mousePos.y < verteces[7]);
		if (hovered)
		{
			color[0] *= 0.7f; color[1] *= 0.7f; color[2] *= 0.7f;
			if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
				pressed = true;
		}
	}

	Batch::GetInstance().Submit(verteces, color, nullptr);

	auto iter = m_Interactable.find(label);
	if (iter != m_Interactable.end())
	{
		if (pressed)
			iter->second = !iter->second;
	}
	else
		m_Interactable.insert(std::make_pair(label, pressed));

	bool flag = m_Interactable.find(label)->second;

	if(flag) Image(height * 0.5f, -height * 0.5f, 0.5f * height, 0.5f * height, m_TriangleDownTexture);
	else     Image(height * 0.5f, -height * 0.5f, 0.5f * height, 0.5f * height, m_TriangleRightTexture);

	m_RelatedPanelProperties.m_YOffset -= (height * 0.5f + m_RelatedPanelProperties.m_Padding.y);

	AlignVertically tempAlign = m_YAlign;
	m_YAlign = AlignVertically::YCENTER;
	Text(label, true, height, -height * 0.5f, height * 0.5f);
	m_YAlign = tempAlign;

	m_RelatedPanelProperties.m_YOffset += height - m_LastTextProperties.m_Size.y;

	return flag;
}

bool Gui::IsPanelHovered()
{
	glm::dvec2 mousePos = ViewPort::GetInstance().GetStaticMousePosition();
	bool flag = (mousePos.x > m_CurrentPanelCoods[0] && mousePos.x < m_CurrentPanelCoods[2] && mousePos.y > m_CurrentPanelCoods[1] && mousePos.y < m_CurrentPanelCoods[7]);
	if (flag == true) m_IsAnyPanelHovered = true;
	return flag;
}

void Gui::RelateToPanel()
{
	m_RelatedPanelProperties.m_Pos = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_Size = glm::vec2(0.0f);
	//relatedPanelProperties.margin = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_YOffset = 0.0f;
	m_IsRelatedToPanel = true;
}

void Gui::UnRelateToPanel()
{
	m_IsRelatedToPanel = false;
	m_RelatedPanelProperties.m_Pos = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_Size = glm::vec2(0.0f);
	//relatedPanelProperties.margin = glm::vec2(0.0f);
	m_RelatedPanelProperties.m_YOffset = 0.0f;
}

void Gui::Begin() const
{
	Batch::GetInstance().m_TIndexCount = 0;
	Batch::GetInstance().BeginText();
}

void Gui::End() const
{
	Batch::GetInstance().EndText();
}

inline void Gui::FontBind(Font * font)
{
	this->m_Font = font;
}

void Gui::LoadStandartFonts()
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

void Gui::ApplyRelatedToPanelProperties(float* x, float* y)
{
	//if (!m_IsRelatedToPanel) return;
	*x += m_RelatedPanelProperties.m_Pos.x + m_RelatedPanelProperties.m_Margin.x;
	*y += m_RelatedPanelProperties.m_Pos.y - m_RelatedPanelProperties.m_Margin.y;
	if (m_RelatedPanelProperties.m_AutoAllignment)
		*y -= m_RelatedPanelProperties.m_YOffset; //NEED to test, not sure that it will work with all UI elements
}

void Gui::FindCharInFont(std::vector<Character*>& localCharV, wchar_t c)
{
	auto cIter = m_Font->m_Characters.find(c);
	if (cIter != m_Font->m_Characters.end())
		localCharV.push_back(cIter->second);
}

void Gui::RecalculateProjectionMatrix()
{
	float aspectRatio = Window::GetInstance().GetCamera().m_Ratio;
	m_ViewProjecTionMat4RelatedToCamera = glm::ortho(-aspectRatio * (float)g_ScaleUI, aspectRatio * (float)g_ScaleUI, (float)-g_ScaleUI, (float)g_ScaleUI, -1.0f, 1.0f);
}

void Gui::ShutDown()
{
	for (size_t i = 0; i < m_StandartFonts.size(); i++)
	{
		delete m_StandartFonts[i];
	}
	m_StandartFonts.clear();
}