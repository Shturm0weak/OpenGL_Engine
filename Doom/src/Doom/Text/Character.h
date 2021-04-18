#pragma once

namespace Doom {

	class Font;
	class Character;

	class DOOM_API Character {
	private:

		float m_Mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

		glm::vec4 m_Color = glm::vec4(1.0f);
		glm::vec2 m_Position;
		glm::vec2 m_Scale;

		float m_XOffset;
		float m_YOffset;
		float m_XAdvance;
		int m_Id;
		int m_X;
		int m_Y;
		int m_Width;
		int m_Height;
		
		friend class Gui;
		friend class Batch;
		friend class Font;
	public:

		Font* m_Font = nullptr;
		int m_IsRelatedToCam = 0;
		wchar_t m_Ch;

		~Character();
		Character();

		void Init(Font* font, wchar_t ch, float x = 0, float y = 0, float scale = 1);
		void Scale(float scale);

		inline float GetRightBottomX() const { return (m_Mesh2D[8] + m_Position.x); }
		inline float GetLeftBottomX() const { return (m_Mesh2D[0] + m_Position.x); }
		inline float GetWidth() const { return (abs(m_Mesh2D[0]) + abs(m_Mesh2D[4])); }
		inline float GetXOffset() const { return m_XOffset; }
	};

	class DOOM_API Font {
	private:

		Character* m_CharacterPtr = nullptr;
		wchar_t* m_Id;
		int* m_X;
		int* m_Y;
		int* m_Width;
		int* m_Height;
		int* m_XOffset;
		int* m_YOffset;
		int* m_XAdvance;
	public:

		std::unordered_map<wchar_t, Character*> m_Characters;
		Texture* m_FontAtlas = nullptr;
		std::string m_Name = "";
		unsigned int m_Count = 0;
		unsigned int m_Size = 0;

		void LoadFont(const std::string& filename, const std::string& pathToTexture);
		void LoadCharacters();

		Font() {}
		~Font();
	
		friend class Character;
	};

}