#pragma once
#include "Character.h"
#include "Render/Batch.h"
#include <stdarg.h>

namespace Doom {

	class DOOM_API Gui {
	public:

		struct PanelStruct {
		public:

			bool m_IsHovered = false;
			float m_YScrollOffset = 0;
		};

		struct UIProperties {
		public:

			glm::vec2 m_Pos = glm::vec2(0.0f);
			glm::vec2 m_Size = glm::vec2(0.0f);
			glm::vec2 m_PanelPosForShader;
			glm::vec2 m_PanelSizeForShader;
			glm::vec2 m_Margin = glm::vec2(10.0f);
			glm::vec2 m_Padding = glm::vec2(10.0f);
			float m_YOffset = 0.0f;
			bool m_AutoAllignment = false;
		};

		struct TextProperties {
		public:

			float m_Width = 0.4f;
			float m_Edge = 0.1f;
			float m_BorderWidth = 0.0f;
			float m_BorderEdge = 0.01f;
			glm::vec4 m_OutLineColor = COLORS::White;
			glm::vec2 m_ShadowOffset = glm::vec2(0.0f);
		};

		//If IsRelatedToPanel set to true then all of ui element's coordinates set to 0
		//will be create at the top left corner of the panel
		UIProperties m_RelatedPanelProperties;
		UIProperties m_LastTextProperties;

		glm::mat4 m_ViewProjecTionMat4RelatedToCamera;

		std::map <std::string, bool> m_Interactable;
		std::map <std::string, PanelStruct> m_Panels;

		bool m_IsInteracting = false;
		bool m_IsAnyPanelHovered = false;
		float m_EdgeRadius = 0;
		TextProperties m_TextProps;
		Font* m_Font;

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

		AlignHorizontally m_XAlign = LEFT;
		AlignVertically m_YAlign = TOP;

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
		void RecalculateProjectionMatrix();
		void ShutDown();

		inline void FontBind(Font* font);
		inline std::vector<Font*>& GetStandartFonts() {return m_StandartFonts;}
	private:
		
		bool m_IsRelatedToPanel = false;
		float m_CharacterXOffset = 0;
		float m_CurrentPanelCoods[8];

		Character* m_Character = nullptr;

		glm::vec4 m_BtnColor = COLORS::Gray;
		glm::vec2 m_Position;
		std::string m_S;
		std::vector<Font*> m_StandartFonts;

		Texture* m_CheckBoxTextureTrue = Texture::Create("src/UIimages/CheckMarkTrue.png");
		Texture* m_CheckBoxTextureFalse = Texture::Create("src/UIimages/CheckMarkFalse.png");
		Texture* m_TriangleRightTexture = Texture::Create("src/UIimages/triangleRight.png");
		Texture* m_TriangleDownTexture = Texture::Create("src/UIimages/triangleDown.png");

		void LoadStandartFonts();
		void ApplyRelatedToPanelProperties(float* x, float* y);

		Gui() {RecalculateProjectionMatrix();}
		~Gui() {}

		friend class EntryPoint;
		friend class Batch;
	};

}