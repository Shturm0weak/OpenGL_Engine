#pragma once
#include "Character.h"
#include "Batch.h"
#include <stdarg.h>

namespace Doom {

	class DOOM_API Gui {
	public:
		void Text(Font* font, std::string str, int m_static = 1, float x = 0, float y = 0, float scale = 12, glm::vec4 color = COLORS::White, int charsAfterComma = 0, ...);
		static Gui* GetInstance() { static Gui gui; return &gui; }

		Font* font;

	private:
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
	};

}