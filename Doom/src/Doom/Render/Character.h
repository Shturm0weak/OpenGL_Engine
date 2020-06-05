#ifndef CHARACTER_H_
#define CHARACTER_H_
#include "../pch.h"
#include "ViewPort.h"

namespace Doom {

	class Font;
	class Character;

	class DOOM_API Character {
		struct Position {
			float x = 0;
			float y = 0;
		};
		struct Scale {
			float x = 0;
			float y = 0;
		};


		//glm::mat4 scale = glm::mat4(1.f);
		//glm::mat4 pos = glm::mat4(1.f);
		//glm::mat4 viewXscale = glm::mat4(1.f);
		//glm::mat4 MVP = glm::mat4(1.f);
		//glm::mat4 view = glm::mat4(1.f);

		unsigned int indeces2D[6] = { 0,1,2,3,2,0 };
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
		glm::mat4 ViewProjecTionRelatedToScreen;
		glm::vec4 color = COLORS::White;
		Texture* texture = nullptr;
		Shader* shader = nullptr;
		Position position;
		Scale _scale;

		friend class Gui;
		friend class Doom::Batch;
	public:
		int isRelatedToCam = 0;
		~Character();
		char ch;
		Font* font = nullptr;
		void Changecharater(Font* font, char ch);
		Character(Font* font, char ch, float x = 0, float y = 0, float scale = 1);
		//void OnRunning(OrthographicCamera& camera);
		/*void Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) {
			va.Bind();
			ib.Bind();
			glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr);
		}*/
		void InitShader();
		void Scale(float scale);
		void Translate(float x, float y);
		inline float GetRightBottomX() { return (mesh2D[8] + position.x); }
		inline float GetLeftBottomX() { return (mesh2D[0] + position.x); }
		inline float GetWidth() { return (abs(mesh2D[0]) + abs(mesh2D[4])); }
		inline float GetXOffset() { return xoffset / 40.; }
		Character* operator=(Character* ch);
	};

	class DOOM_API Font {
	public:
		Font() {
			shader = new Shader("src/Shaders/Text.shader");
		}
		~Font() {
			delete fontAtlas;
			delete shader;
			delete[] id;
			delete[] x;
			delete[] y;
			delete[] width;
			delete[] height;
			delete[] xoffset;
			delete[] yoffset;
			delete[] xadvance;
		}
		std::vector<Character*> characters;
		Texture* fontAtlas = nullptr;
		std::string name = "";
		Shader* shader = nullptr;
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

		void LoadFont(std::string filename, std::string pathToTexture) {
			fontAtlas = new Texture(pathToTexture);
			std::ifstream in_file;
			in_file.open(filename);
			if (in_file.is_open()) {
				in_file >> name;
				in_file >> count;
				count++;
				in_file >> size;
				id = new unsigned int[count];
				x = new int[count];
				y = new int[count];
				width = new int[count];
				height = new int[count];
				xoffset = new int[count];
				yoffset = new int[count];
				xadvance = new int[count];
				for (unsigned int i = 0; i < count; i++)
				{
					//std::cout << i << "	";
					in_file >> id[i];
					//std::cout << id[i] << "	";
					in_file >> x[i];
					//std::cout << x[i] << "	";
					in_file >> y[i];
					//std::cout << y[i] << "	";
					in_file >> width[i];
					//std::cout << width[i] << "	";
					in_file >> height[i];
					//std::cout << height[i] << "	";
					in_file >> xoffset[i];
					//std::cout << xoffset[i] << "	";
					in_file >> yoffset[i];
					//std::cout << yoffset[i] << "	";
					in_file >> xadvance[i];
					//std::cout << xadvance[i] << std::endl;
				}
			}
			in_file.close();
			std::cout << BOLDGREEN << "Font " << name <<" has been loaded" << RESET << std::endl;
		}

		void LoadCharacters() {
			Character* character = nullptr;
			for (unsigned int i = 0; i < count; i++)
			{
				character = (new Character(this, id[i]));
				characters.push_back(character);
				//std::cout << i << "	" << int(characters[i].get().ch) << "	" << characters[i].get().ch << std::endl;
			}
			std::cout << BOLDGREEN << "Characters has been loaded" << RESET << std::endl;
		}
	};

}

#endif // !CHARACTER_H_