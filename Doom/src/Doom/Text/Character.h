#ifndef CHARACTER_H_
#define CHARACTER_H_
#include "../pch.h"
#include "Core/ViewPort.h"

namespace Doom {

	class Font;
	class Character;

	class DOOM_API Character {

		static unsigned int indeces2D[6];
		float mesh2D[20] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
		};

		int id;
		int x;
		int y;
		int width;
		int height;
		float xoffset;
		float yoffset;
		float xadvance;

	private:
		glm::vec4 color = COLORS::White;
	
		Shader* shader = nullptr;
		glm::vec2 position;
		glm::vec2 _scale;

		friend class Gui;
		friend class Doom::Batch;
		friend class Font;
	public:
		int isRelatedToCam = 0;
		char ch;

		Font* font = nullptr;

		~Character();
		Character(Font* font, int ch, float x = 0, float y = 0, float scale = 1);
		void Scale(float scale);
		void Translate(float x, float y);
		inline float GetRightBottomX() const { return (mesh2D[8] + position.x); }
		inline float GetLeftBottomX() const { return (mesh2D[0] + position.x); }
		inline float GetWidth() const { return (abs(mesh2D[0]) + abs(mesh2D[4])); }
		inline float GetXOffset() const { return xoffset; }
	};

	class DOOM_API Font {
	public:
		Font() {}
		~Font();

		std::map<char,Character*> characters;
		Texture* fontAtlas = nullptr;
		std::string name = "";
		static Shader* shader;
		unsigned int count = 0;
		unsigned int size = 0;
		unsigned int *id;
		int *x;
		int *y;
		int *width;
		int *height;
		int *xoffset;
		int *yoffset;
		int *xadvance;

		void LoadFont(const std::string& filename, const std::string& pathToTexture);
		void LoadCharacters();
	};

}

#endif