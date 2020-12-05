#ifndef CHARACTER_H_
#define CHARACTER_H_
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

		static unsigned int s_Indices2D[6];
		float m_Mesh2D[20] = {
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
		friend class Font;
	public:

		int m_IsRelatedToCam = 0;
		char m_Ch;

		Font* m_Font = nullptr;

		~Character();
		Character(Font* font, int ch, float x = 0, float y = 0, float scale = 1);

		void Scale(float scale);
		void Translate(float x, float y);
		inline float GetRightBottomX() const { return (m_Mesh2D[8] + m_Position.x); }
		inline float GetLeftBottomX() const { return (m_Mesh2D[0] + m_Position.x); }
		inline float GetWidth() const { return (abs(m_Mesh2D[0]) + abs(m_Mesh2D[4])); }
		inline float GetXOffset() const { return m_XOffset; }
	};

	class DOOM_API Font {
	public:

		std::map<char,Character*> m_Characters;
		Texture* m_FontAtlas = nullptr;
		std::string m_Name = "";
		static Shader* s_Shader;
		unsigned int m_Count = 0;
		unsigned int m_Size = 0;
		unsigned int *m_Id;
		int *m_X;
		int *m_Y;
		int *m_Width;
		int *m_Height;
		int *m_XOffset;
		int *m_YOffset;
		int *m_XAdvance;

		void LoadFont(const std::string& filename, const std::string& pathToTexture);
		void LoadCharacters();

		Font() {}
		~Font();
	};

}

#endif