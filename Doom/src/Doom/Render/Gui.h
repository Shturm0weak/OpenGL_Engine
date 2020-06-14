#pragma once
#include "Character.h"
#include "Batch.h"
#include <stdarg.h>

namespace Doom {

	struct TextProperties {
	public:
		float width = 0.4;
		float edge = 0.1;
		float borderwidth = 0.f;
		float borderedge = 0.01f;
		glm::vec4 outLineColor = COLORS::White;
		glm::vec2 shadowOffset = glm::vec2(0.000, 0.000);
	};

	class DOOM_API Gui {
	public:

		TextProperties textProps;

		enum AlignHorizontally {
			XCENTER = 1,
			LEFT = 2,
		};

		enum AlignVertically {
			YCENTER = 5,
			TOP = 3,
			BOTTOM = 4,
		};

		enum StandartFonts {
			CALIBRI = 0,
			ARIAL = 1,
			HARRINGTON = 2,
			PEAK = 3,
			PLAYBALL = 4,
			SEGOE = 5,
		};

		AlignHorizontally xAlign = LEFT;
		AlignVertically yAlign = TOP;

		static Gui* GetInstance() { static Gui gui; return &gui; }

		//x,y in pixels
		void Text(std::string str, int m_static = 1, float x = 0, float y = 0, float scale = 12, glm::vec4 color = COLORS::White, int charsAfterComma = 0, ...);
		
		//x,y,width and height in pixels
		bool Button(std::string str, float x = 0, float y = 0,float scale = 24, float width = 100,float height = 50,glm::vec4 btnColor = COLORS::Gray,glm::vec4 pressedBtnColor = COLORS::Gray * 0.5f, glm::vec4 textColor = COLORS::White);

		void Panel(float x = 0, float y = 0, float width = 400, float height = 400,glm::vec4 color = COLORS::Gray,Texture* texture = nullptr);

		void Bar(float x, float y, float value, float maxValue, glm::vec4 color, glm::vec4 outColor, float width, float height);

		void Begin() {
			Batch::GetInstance()->indexcount = 0;
			Batch::GetInstance()->Begin();
		}

		void End() {
			Batch::GetInstance()->End();
		}

		void FontBind(Font* font) {
			this->font = font;
		}

		std::vector<Font*>& GetStandartFonts() {
			return standartFonts;
		}

		Font* font;

	private:
		void LoadStandartFonts();

		glm::vec4 btnColor = COLORS::Gray;
		
		std::vector<Font*> standartFonts;
		Gui() {}
		~Gui() {}
		struct Position {
			float x = 0;
			float y = 0;
		};
		Position position;
		std::string text;
		std::string s;
		float counter = 0;
		int amount = 0;
		int m_charsAfterComma = 0;
		char pchar;
		Character* character = nullptr;

		friend class EntryPoint;
	};

}