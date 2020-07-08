#include "Character.h"

using namespace Doom;

Character::~Character()
{
}

Character::Character(Font* font,int ch,float posx,float posy,float scale)
{
	shader = font->shader;
	this->font = font;
	position.x = posx;
	position.y = posy;
	texture = font->fontAtlas;
	{
		int i = ch;
		this->ch = font->id[i];
		id = font->id[i];
		x = font->x[i];
		y = font->y[i];
		width = font->width[i];
		height = font->height[i];
		xoffset = font->xoffset[i];
		yoffset = font->yoffset[i];
		xadvance = font->xadvance[i];
	}
	{
		double size = font->fontAtlas->GetWidth();

		mesh2D[2] = (x) / size;
		mesh2D[3] = (abs(y - size) - height) / size;
		mesh2D[6] = (x + width) / size;
		mesh2D[7] = (abs(y - size) - height) / size;
		mesh2D[10] = (x + width) / size;
		mesh2D[11] = abs(y - size) / size;
		mesh2D[14] = x / size;
		mesh2D[15] = abs(y - size) / size;

		double widthRatio = Window::GetCamera().GetAspectRatio() / ViewPort::GetInstance()->GetSize()[0];
		double heightRatio = 1 / ViewPort::GetInstance()->GetSize()[1];

		mesh2D[0]  = 0;
		mesh2D[1]  = 0;
		mesh2D[4]  = (widthRatio  * (width  + xoffset));
		mesh2D[5]  = 0;
		mesh2D[8]  = (widthRatio  * (width  + xoffset));
		mesh2D[9]  = (heightRatio * (height + yoffset));
		mesh2D[12] = 0;
		mesh2D[13] = (heightRatio * (height + yoffset));
	}
	this->Scale(scale);
	this->shader->UnBind();
}

void Character::Scale(float argscale)
{
	yoffset *= argscale;
	_scale.x = argscale;
	_scale.y = argscale;
}

inline void Character::Translate(float x, float y)
{
	position.x = x;
	position.y = y;
}

Doom::Font::~Font()
{
	delete[] id;
	delete[] x;
	delete[] y;
	delete[] width;
	delete[] height;
	delete[] xoffset;
	delete[] yoffset;
	delete[] xadvance;

	for (auto i = characters.begin(); i != characters.end(); i++)
	{
		delete i->second;
	}

	characters.clear();
}

void Doom::Font::LoadFont(std::string filename, std::string pathToTexture)
{
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
			//std::cout << id[i] << "	\n";
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
	//std::cout << "count " << count << std::endl;
	in_file.close();
	std::cout << BOLDGREEN << "Font " << name << " has been loaded" << RESET << std::endl;
}

void Doom::Font::LoadCharacters()
{
	Character* character = nullptr;
	for (unsigned int i = 0; i < count; i++)
	{
		character = (new Character(this, i));
		characters.insert(std::make_pair(character->ch, character));
		//std::cout << i << "	" << int(characters[i].get().ch) << "	" << characters[i].get().ch << std::endl;
	}
	std::cout << BOLDGREEN << "Characters has been loaded" << RESET << std::endl;
}
