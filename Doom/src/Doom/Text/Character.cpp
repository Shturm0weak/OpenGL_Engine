#include "Character.h"

using namespace Doom;

Character::~Character()
{
}

Character::Character(Font* font,int ch,float posx,float posy,float scale)
{
	m_Shader = font->s_Shader;
	this->m_Font = font;
	m_Position.x = posx;
	m_Position.y = posy;
	{
		int i = ch;
		this->m_Ch = font->m_Id[i];
		m_Id = font->m_Id[i];
		m_X = font->m_X[i];
		m_Y = font->m_Y[i];
		m_Width = font->m_Width[i];
		m_Height = font->m_Height[i];
		m_XOffset = font->m_XOffset[i];
		m_YOffset = font->m_YOffset[i];
		m_XAdvance = font->m_XAdvance[i];
	}
	{
		double size = font->m_FontAtlas->GetWidth();

		m_Mesh2D[2] = (m_X) / size;
		m_Mesh2D[3] = (abs(m_Y - size) - m_Height) / size;
		m_Mesh2D[6] = (m_X + m_Width) / size;
		m_Mesh2D[7] = (abs(m_Y - size) - m_Height) / size;
		m_Mesh2D[10] = (m_X + m_Width) / size;
		m_Mesh2D[11] = abs(m_Y - size) / size;
		m_Mesh2D[14] = m_X / size;
		m_Mesh2D[15] = abs(m_Y - size) / size;

		double widthRatio = Window::GetCamera().GetAspectRatio() / ViewPort::GetInstance()->GetSize()[0];
		double heightRatio = 1 / ViewPort::GetInstance()->GetSize()[1];

		m_Mesh2D[0]  = 0;
		m_Mesh2D[1]  = 0;
		m_Mesh2D[4]  = (widthRatio  * (m_Width  + m_XOffset));
		m_Mesh2D[5]  = 0;
		m_Mesh2D[8]  = (widthRatio  * (m_Width  + m_XOffset));
		m_Mesh2D[9]  = (heightRatio * (m_Height + m_YOffset));
		m_Mesh2D[12] = 0;
		m_Mesh2D[13] = (heightRatio * (m_Height + m_YOffset));
	}
	this->Scale(scale);
	this->m_Shader->UnBind();
}

void Character::Scale(float argscale)
{
	m_YOffset *= argscale;
	m_Scale.x = argscale;
	m_Scale.y = argscale;
}

inline void Character::Translate(float x, float y)
{
	m_Position.x = x;
	m_Position.y = y;
}

Doom::Font::~Font()
{
	delete[] m_Id;
	delete[] m_X;
	delete[] m_Y;
	delete[] m_Width;
	delete[] m_Height;
	delete[] m_XOffset;
	delete[] m_YOffset;
	delete[] m_XAdvance;

	for (auto i = m_Characters.begin(); i != m_Characters.end(); i++)
	{
		delete i->second;
	}

	m_Characters.clear();
}

void Doom::Font::LoadFont(const std::string& filename, const std::string& pathToTexture)
{
	this->m_FontAtlas = Texture::Create(pathToTexture);
	
	std::ifstream in_file;
	in_file.open(filename);
	if (in_file.is_open()) {
		in_file >> m_Name;
		in_file >> m_Count;
		m_Count++;
		in_file >> m_Size;
		m_Id = new unsigned int[m_Count];
		m_X = new int[m_Count];
		m_Y = new int[m_Count];
		m_Width = new int[m_Count];
		m_Height = new int[m_Count];
		m_XOffset = new int[m_Count];
		m_YOffset = new int[m_Count];
		m_XAdvance = new int[m_Count];
		for (unsigned int i = 0; i < m_Count; i++)
		{
			//std::cout << i << "	";
			in_file >> m_Id[i];
			//std::cout << id[i] << "	\n";
			in_file >> m_X[i];
			//std::cout << x[i] << "	";
			in_file >> m_Y[i];
			//std::cout << y[i] << "	";
			in_file >> m_Width[i];
			//std::cout << width[i] << "	";
			in_file >> m_Height[i];
			//std::cout << height[i] << "	";
			in_file >> m_XOffset[i];
			//std::cout << xoffset[i] << "	";
			in_file >> m_YOffset[i];
			//std::cout << yoffset[i] << "	";
			in_file >> m_XAdvance[i];
			//std::cout << xadvance[i] << std::endl;
		}
	}
	in_file.close();
	std::cout << BOLDGREEN << "Font: <" << NAMECOLOR << m_Name << BOLDGREEN << "> has been loaded" << RESET << std::endl;
}

void Doom::Font::LoadCharacters()
{
	Character* character = nullptr;
	for (unsigned int i = 0; i < m_Count; i++)
	{
		character = (new Character(this, i));
		m_Characters.insert(std::make_pair(character->m_Ch, character));
	}
	//std::cout << BOLDGREEN << "Characters has been loaded" << RESET << std::endl;
}
