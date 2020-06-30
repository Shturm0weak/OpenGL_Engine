#pragma once
#include "Character.h"
#include "Batch.h"
#include <stdarg.h>

namespace Doom {

	class DOOM_API Gui {
	public:

		struct UIProperties {
		public:
			glm::vec2 pos = glm::vec2(0.0f);
			glm::vec2 size = glm::vec2(0.0f);
			float yOffset = 0.0f;
			glm::vec2 margin = glm::vec2(0.0f);
			glm::vec2 padding = glm::vec2(0.0f);
			bool autoAllignment = false;
		};

		struct TextProperties {
		public:
			float width = 0.4;
			float edge = 0.1;
			float borderwidth = 0.f;
			float borderedge = 0.01f;
			glm::vec4 outLineColor = COLORS::White;
			glm::vec2 shadowOffset = glm::vec2(0.000, 0.000);
		};

		glm::mat4 ViewProjecTionRelatedToCamera;
		float edgeRadius = 0;
		TextProperties textProps;

		//If IsRelatedToPanel set to true then all of ui element's coordinates set to 0
		//will be create at the top left corner of the panel
		UIProperties relatedPanelProperties;
		UIProperties lastTextProperties;

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
		void Text(std::string text, int m_static = 1, float x = 0, float y = 0, float scale = 12, glm::vec4 color = COLORS::White, int charsAfterComma = 0, ...);
		
		//x,y,width and height in pixels
		bool Button(std::string label, float x = 0, float y = 0,float scale = 24, float width = 100,float height = 50,glm::vec4 btnColor = COLORS::Gray,glm::vec4 pressedBtnColor = COLORS::Gray * 0.5f, glm::vec4 textColor = COLORS::White,Texture* texture = nullptr);

		void Panel(float x = 0, float y = 0, float width = 400, float height = 400,glm::vec4 color = COLORS::Gray,Texture* texture = nullptr);

		void Bar(float x, float y, float value, float maxValue, glm::vec4 color, glm::vec4 outColor, float width, float height);

		bool CheckBox(std::string label,bool* value, float x = 0.0f, float y = 0.0f, float size = 0.0f,glm::vec4 textColor = COLORS::White,glm::vec4 imageColor = COLORS::White);

		float SliderFloat(std::string label, float* value,float min = 0.0f,float max = 1.0f,float x = 0.0f,float y = 0.0f, float width = 100.0f,float height = 50.0f,glm::vec4 sliderColor = glm::vec4(0.3, 0.3, 0.3, 1),glm::vec4 panelColor = glm::vec4(0.5,0.5,0.5,1));

		bool IsPanelHovered();

		void RelateToPanel();
		void UnRelateToPanel();

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

		inline std::vector<Font*>& GetStandartFonts() {return standartFonts;}

		Font* font;

		void RecalculateProjectionMatrix() {
			float aspectRatio = Window::GetCamera().GetAspectRatio();
			ViewProjecTionRelatedToCamera = glm::ortho(-aspectRatio * (float)WIDTH, aspectRatio * (float)WIDTH, (float)-HEIGHT, (float)HEIGHT, -1.0f, 1.0f);
		}

		void ShutDown();

	private:
		void LoadStandartFonts();
		void ApplyRelatedToPanelProperties(float* x, float* y);

		glm::vec4 btnColor = COLORS::Gray;
		
		std::vector<Font*> standartFonts;

		Gui() {
			RecalculateProjectionMatrix();
		}
		~Gui() {}

		Texture* checkBoxTextureTrue = new Texture("src/UIimages/CheckMarkTrue.png");
		Texture* checkBoxTextureFalse = new Texture("src/UIimages/CheckMarkFalse.png");
		bool IsRelatedToPanel = false;
		glm::vec2 position;
		std::string s;
		float characterXoffset = 0;
		float currentPanelCoods[8];
		Character* character = nullptr;

		friend class EntryPoint;
	};

}