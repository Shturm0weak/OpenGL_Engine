#include "../pch.h"
#include "Texture.h"
#include "stb_image.h"

using namespace Doom;

Texture::Texture(const std::string& path, int flip)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
	m_height(0), m_width(0), m_BPP(0)
{
	for (unsigned int i = 0; i < texturesArray.size(); i++)
	{
		if (path == texturesArray[i]->m_FilePath) {
			m_RendererID = texturesArray[i]->m_RendererID;
			m_BPP = texturesArray[i]->m_BPP;
			m_height = texturesArray[i]->m_height;
			m_width = texturesArray[i]->m_width;
			m_FilePath = texturesArray[i]->m_FilePath;
			auto it = Texturesused.find(m_RendererID);
			if (it != Texturesused.end())
				(*it).second++;
			isExisted = true;
			return;
		}
	}

	stbi_set_flip_vertically_on_load(flip);
	m_LocalBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_BPP, 4);
	
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);
	if (m_LocalBuffer)
		stbi_image_free(m_LocalBuffer);
	texturesArray.push_back(this);
	bindedAmount++;
	VRAMused += (m_height * m_width * 4) * 0.000001;
	auto it = Texturesused.find(m_RendererID);
	if (it == Texturesused.end())
		Texturesused.insert(std::make_pair(m_RendererID, 1));
	else
		(*it).second++;
	//std::cout << "\n\nnew texture " << path << std::endl;
}

Texture::Texture()
{
	m_FilePath = "None";
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	uint32_t color = 0xffffffff;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);
	glBindTexture(GL_TEXTURE_2D, 0);
	VRAMused += 0.000004;
}

Texture::Texture(std::string path,bool IsEmpty)
:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
	m_height(0), m_width(0), m_BPP(0)
{


	stbi_set_flip_vertically_on_load(1);
	m_LocalBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_BPP, 4);

	for (unsigned int i = 0; i < texturesArray.size(); i++)
	{
		if (path == texturesArray[i]->m_FilePath) {
			m_RendererID = texturesArray[i]->m_RendererID;
			m_BPP = texturesArray[i]->m_BPP;
			m_height = texturesArray[i]->m_height;
			m_width = texturesArray[i]->m_width;
			m_FilePath = texturesArray[i]->m_FilePath;
			auto it = Texturesused.find(m_RendererID);
			if (it != Texturesused.end())
				(*it).second++;
			isExisted = true;
			return;
		}
	}
}

Texture::~Texture() {
	auto it = Texturesused.find(m_RendererID);
	if (it != Texturesused.end()) {
		it->second--;
		//std::cout << "\n\nDelete texture " << m_FilePath << " used " << it->second << std::endl;
		if (it->second < 0) {
			Texturesused.erase(it);
			glDeleteTextures(1, &m_RendererID);
			bindedAmount--;
		}
	}
	unsigned int size = texturesArray.size();
	for (unsigned int i = 0; i < size; i++)
	{
		if (texturesArray[i] == this) {
			texturesArray.erase(texturesArray.begin() + i);
			break;
		}
	}
}

void Texture::GenTexture()
{
	if (!isExisted) {
		glGenTextures(1, &m_RendererID);
		glBindTexture(GL_TEXTURE_2D, m_RendererID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
		glBindTexture(GL_TEXTURE_2D, 0);
		if (m_LocalBuffer)
			stbi_image_free(m_LocalBuffer);
		texturesArray.push_back(this);
		bindedAmount++;
		auto it = Texturesused.find(m_RendererID);
		if (it == Texturesused.end())
			Texturesused.insert(std::make_pair(m_RendererID, 1));
		else
			(*it).second++;
	}
}

void Texture::Bind(unsigned int slot) const {
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::UnBind() const{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Doom::Texture::ShutDown()
{
	Texturesused.clear();
	unsigned int size = texturesArray.size();
	uint32_t counter = 0;
	for (unsigned int i = 0; i == size; i++)
	{
		delete texturesArray[counter];
	}
	texturesArray.clear();
}
