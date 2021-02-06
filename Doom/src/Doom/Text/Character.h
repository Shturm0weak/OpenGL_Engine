#pragma once

#include "../pch.h"
#include "Core/ViewPort.h"

namespace Doom {

	class Font;
	class Character;

	class DOOM_API Character {
	private:

		Shader* m_Shader = nullptr;
		glm::vec2 m_Position;
		glm::vec2 m_Scale;
		glm::vec4 m_Color = COLORS::White;

		float m_Mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

		int m_Id;
		int m_X;
		int m_Y;
		int m_Width;
		int m_Height;
		float m_XOffset;
		float m_YOffset;
		float m_XAdvance;
		
		friend class Gui;
		friend class Doom::Batch;
		friend class Doom::Font;
	public:

		int m_IsRelatedToCam = 0;
		char m_Ch;

		Font* m_Font = nullptr;

		~Character();
		Character();

		void Init(Font* font, int ch, float x = 0, float y = 0, float scale = 1);
		void Scale(float scale);

		inline float GetRightBottomX() const { return (m_Mesh2D[8] + m_Position.x); }
		inline float GetLeftBottomX() const { return (m_Mesh2D[0] + m_Position.x); }
		inline float GetWidth() const { return (abs(m_Mesh2D[0]) + abs(m_Mesh2D[4])); }
		inline float GetXOffset() const { return m_XOffset; }
	};

	class DOOM_API Font {
	private:

		Character* m_CharacterPtr = nullptr;
		unsigned int* m_Id;
		int* m_X;
		int* m_Y;
		int* m_Width;
		int* m_Height;
		int* m_XOffset;
		int* m_YOffset;
		int* m_XAdvance;
	public:

		std::map<char,Character*> m_Characters;
		Texture* m_FontAtlas = nullptr;
		std::string m_Name = "";
		static Shader* s_Shader;
		unsigned int m_Count = 0;
		unsigned int m_Size = 0;

		void LoadFont(const std::string& filename, const std::string& pathToTexture);
		void LoadCharacters();

		Font() {}
		~Font();
	
		friend class Doom::Character;
	};

}