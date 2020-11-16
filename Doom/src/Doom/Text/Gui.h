#pragma once
#include "Character.h"
#include "Render/Batch.h"
#include <stdarg.h>

namespace Doom {

	class DOOM_API Gui {
	public:

		struct PanelStruct {
		public:
			bool isHovered = false;
			float yScrollOffset = 0;
		};

		struct UIProperties {
		public:
			glm::vec2 pos = glm::vec2(0.0f);
			glm::vec2 size = glm::vec2(0.0f);
			float yOffset = 0.0f;
			glm::vec2 margin = glm::vec2(10.0f);
			glm::vec2 padding = glm::vec2(10.0f);
			bool autoAllignment = false;

			glm::vec2 panelPosForShader;
			glm::vec2 panelSizeForShader;
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

		//If IsRelatedToPanel set to true then all of ui element's coordinates set to 0
		//will be create at the top left corner of the panel
		UIProperties relatedPanelProperties;
		UIProperties lastTextProperties;

		glm::mat4 ViewProjecTionRelatedToCamera;

		std::map <std::string, bool> interactables;
		std::map <std::string, PanelStruct> panels;

		bool isInteracting = false;
		bool isAnyPanelHovered = false;
		float edgeRadius = 0;
		TextProperties textProps;
		Font* font;

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
		void Text(std::string text, int m_static = 1, float x = 0, float y = 0, float fontScale = 20, glm::vec4 color = COLORS::White, int charsAfterComma = 0, ...);
		
		//x,y,width and height in pixels
		bool Button(std::string label, float x = 0, float y = 0,float scale = 24, float width = 100,float height = 50,glm::vec4 btnColor = COLORS::Gray,glm::vec4 pressedBtnColor = COLORS::Gray * 0.5f, glm::vec4 textColor = COLORS::White,Texture* texture = nullptr);

		void Panel(std::string label, float x = 0, float y = 0, float width = 400, float height = 400,glm::vec4 color = COLORS::Gray,bool changeColorWhenHovered = false,Texture* texture = nullptr);

		void Bar(float x, float y, float value, float maxValue, glm::vec4 color, glm::vec4 outColor, float width, float height);

		bool CheckBox(std::string label,bool* value, float x = 0.0f, float y = 0.0f, float size = 0.0f,glm::vec4 textColor = COLORS::White,glm::vec4 imageColor = COLORS::White);

		float SliderFloat(std::string label, float* value,float min = 0.0f,float max = 1.0f,float x = 0.0f,float y = 0.0f, float width = 100.0f,float height = 50.0f,glm::vec4 sliderColor = glm::vec4(0.3, 0.3, 0.3, 1),glm::vec4 panelColor = glm::vec4(0.5,0.5,0.5,1));

		void Image(float x = 0.0f, float y = 0.0f,float width = 100.0f,float height = 100.0f,Texture* texture = nullptr,glm::vec4 color = COLORS::White);

		bool CollapsingHeader(std::string label,float x,float y,vec4 color);

		bool IsPanelHovered();

		void RelateToPanel();
		void UnRelateToPanel();

		void Begin() const;
		void End() const;

		inline void FontBind(Font* font);
		inline std::vector<Font*>& GetStandartFonts() {return standartFonts;}

		void RecalculateProjectionMatrix();

		void ShutDown();

	private:
		
		Texture* checkBoxTextureTrue = Texture::Create("src/UIimages/CheckMarkTrue.png");
		Texture* checkBoxTextureFalse = Texture::Create("src/UIimages/CheckMarkFalse.png");
		Texture* triangleRightTexture = Texture::Create("src/UIimages/triangleRight.png");
		Texture* triangleDownTexture = Texture::Create("src/UIimages/triangleDown.png");

		void LoadStandartFonts();

		void ApplyRelatedToPanelProperties(float* x, float* y);

		glm::vec4 btnColor = COLORS::Gray;
		glm::vec2 position;
		std::string s;
		std::vector<Font*> standartFonts;

		Gui() {RecalculateProjectionMatrix();}
		~Gui() {}

		bool IsRelatedToPanel = false;

		float characterXoffset = 0;
		float currentPanelCoods[8];

		Character* character = nullptr;

		friend class EntryPoint;
		friend class Batch;
	};

}