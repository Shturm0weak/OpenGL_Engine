#include "Text.h"
#include <functional>

using namespace Doom;

void Gui::Text(Font* font, std::string str, int m_static, float x, float y, float scale, glm::vec4 color , int charsAfterComma, ...)
{
	x *= 10;
	y *= 10;
	this->font = font;
	text = str;
	m_charsAfterComma = charsAfterComma;
	int counter1 = 0;
	bool border = false;
	va_list argptr;
	va_start(argptr, charsAfterComma);
	for (size_t i = 0; i < str.size(); i++)
	{
		if (str[i] == '%') {
			if (str[i + 1] == 'd')
				s = std::to_string(va_arg(argptr, int));
			else if (str[i + 1] == 'f') {
				s = std::to_string(va_arg(argptr, double));
			}
			else if (str[i + 1] == 's')
				s = (va_arg(argptr, std::string));
			for (unsigned int j = 0; j < s.size(); j++)
			{
				pchar = s[j];
				if (pchar == '.') {
					border = true;
					pchar = ',';
				}
				if (border == true)
					counter1++;
				if (charsAfterComma == 0 && pchar == ',')
					break;
				
				for (size_t i = 0; i < font->characters.size(); i++)
				{
					if (font->characters[i]->ch == pchar) {
						character = font->characters[i];
						break;
					}
				}
				amount++;
				character->isRelatedToCam = m_static;
				character->_scale.x = scale;
				character->_scale.y = scale;
				character->color = color;

				if (i == 0) {
					counter = 0;
					character->Translate(x + counter + character->GetWidth() * scale * 0.5, y );
				}
				else {
					character->Translate(x + counter + character->GetWidth() * scale * 0.5, y );
				}
				counter += (character->xadvance + character->xoffset)* scale * 0.019230769;
				//character->OnRunning(Window::GetCamera());
				Batch::GetInstance()->Submit(*character);
				
				if (counter1 > charsAfterComma)
					break;
			}
			counter1 = 0;
			border = false;
			i++;
		}
		else if (str[i] == '\n') {
			y -=2 * scale;
			counter = 0;
			continue;
		}
		else {
			if (str[i] == '.')
				str[i] = ',';
			for (size_t k = 0; k < font->characters.size(); k++)
			{
				if (font->characters[k]->ch == str[i]) {
					character = font->characters[k];
					break;
				}
			}
			amount++;
			character->isRelatedToCam = m_static;
			character->_scale.x = scale;
			character->_scale.y = scale;
			character->color = color;
			
			if (i == 0) {
				counter = 0;
				character->Translate(x + counter + character->GetWidth() * scale * 0.5, y);
			}
			else {
				character->Translate(x + counter + character->GetWidth() * scale * 0.5, y);
			}
			counter += (character->xadvance + character->xoffset)* scale * 0.019230769;
			//character->OnRunning(Window::GetCamera());
			Batch::GetInstance()->Submit(*character);
		}
	}
	va_end(argptr);
}
