#include "../pch.h"
#include "Texture.h"
#include "stb_image.h"

using namespace Doom;

Texture::Texture(const std::string& path, int flip)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
	m_height(0), m_width(0), m_BPP(0)
{
	auto iter = textures.find(path);
	if(iter == textures.end())
		textures.insert(std::make_pair(path, this));
	else {
#ifdef _DEBUG
		std::cout << YELLOW << "Texture with this file path: " << path <<  " already exists\n" << RESET;
#endif
		return;
	}
	stbi_set_flip_vertically_on_load(flip);
	m_LocalBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_BPP, 4);
	
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);
	if (m_LocalBuffer)
		UnloadFromRAM(m_FilePath);
	bindedAmount++;
	VRAMused += (m_height * m_width * 4 * 4) * 0.000001;
}

Texture::Texture()
{
}

Texture::~Texture() {
	auto it = textures.find(m_FilePath);
	if (it != textures.end()) {
		textures.erase(it);
		UnloadFromRAM(m_FilePath);
		UnloadFromVRAM(m_FilePath);
		bindedAmount--;
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
	std::vector<Texture*> ts;
	for (auto i = textures.begin(); i != textures.end(); i++)
	{
		ts.push_back(i->second);
	}
	textures.clear();
	for (uint32_t i = 0; i < ts.size(); i)
	{
		delete ts[i];
		ts.erase(ts.begin() + i);
	}
}

Texture * Doom::Texture::Get(std::string filePath,bool showErrors)
{
	auto iter = textures.find(filePath);
	if (iter != textures.end()) {
		return iter->second;
	}
	else {
		if(showErrors)
			std::cout << BOLDYELLOW << "Texture at file path: " << filePath <<  " doesn't exist!" << RESET << std::endl;
		return nullptr;
	}
}

void Doom::Texture::UnloadFromRAM(std::string filePath)
{
	Texture* t = Get(filePath,false);
	if (t != nullptr && t->m_LocalBuffer) {
		stbi_image_free(t->m_LocalBuffer);
		t->m_LocalBuffer = nullptr;
	}
}

void Doom::Texture::UnloadFromVRAM(std::string filePath)
{
	Texture* t = Get(filePath,false);
	if (t != nullptr || t != NULL) {
		VRAMused -= (t->m_height * t->m_width * 4 * 4) * 0.000001;
		glDeleteTextures(1, &t->m_RendererID);
		t->m_RendererID = -1;
	}
}

Texture * Doom::Texture::ColoredTexture(std::string name, uint32_t color)
{
	auto iter = textures.find(name);
	if (iter != textures.end()) {
#ifdef _DEBUG
		std::cout << BOLDYELLOW << "Texture at file path: " << name << " already exists\n" << RESET;
#endif
		return iter->second;
	}
	Texture* t = new Texture();
	t->m_FilePath = name;
	glGenTextures(1, &t->m_RendererID);
	glBindTexture(GL_TEXTURE_2D, t->m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);
	glBindTexture(GL_TEXTURE_2D, 0);
	VRAMused += 0.000004;
	textures.insert(std::make_pair(t->m_FilePath, t));
	return t;
}

Texture * Doom::Texture::Create(std::string filePath,bool flip)
{
	Texture* t = Get(filePath,false);
	if (t == nullptr || t == NULL)
		return new Texture(filePath, flip);
	else
		return t;
}

void Doom::Texture::LoadTextureInRAM(std::string filePath, bool flip)
{
	Texture* t = nullptr;
	auto iter = textures.find(filePath);
	if (iter == textures.end()) {
		t = new Texture();
		t->m_FilePath = filePath;
		textures.insert(std::make_pair(t->m_FilePath, t));
	}
	else {
		t = iter->second;
	}
	if (t->m_LocalBuffer == nullptr) {
		stbi_set_flip_vertically_on_load(flip);
		t->m_LocalBuffer = stbi_load(t->m_FilePath.c_str(), &t->m_width, &t->m_height, &t->m_BPP, 4);
	}
	else {
#ifdef _DEBUG
		std::cout << BOLDYELLOW << "m_localBuffer is not empty!\n" << RESET;
#endif
	}
}

void Doom::Texture::LoadTextureInVRAM(std::string filePath, bool unloadFromRam)
{
	Texture* t = Get(filePath);
	if (t == nullptr)
		return;
	if(t->m_LocalBuffer == nullptr){
#ifdef _DEBUG
		std::cout << RED << "Can't load texture in VRAM, m_LocalBuffer is unloaded from RAM!\n" << RESET;
#endif
		return;
	}
	if (t->m_RendererID != -1) {
#ifdef _DEBUG
		std::cout << BOLDYELLOW << "Texture is already in VRAM\n" << RESET;
#endif
		return;
	}
	glGenTextures(1, &t->m_RendererID);
	glBindTexture(GL_TEXTURE_2D, t->m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->m_width, t->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, t->m_LocalBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);
	if (unloadFromRam) {
		UnloadFromRAM(t->m_FilePath);
	}
	bindedAmount++;
	VRAMused += (t->m_height * t->m_width * 4 * 4) * 0.000001;
}

unsigned int Doom::Texture::LoadCubeMap(std::vector<std::string> faces)
{
	unsigned int m_RendererID;
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_RendererID);

	int width, height, nChannles;
	for (size_t i = 0; i < faces.size(); i++)
	{
		stbi_set_flip_vertically_on_load(false);
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nChannles, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else {
			std::cout << RED << "Cube map texture failed to load at path: " + faces[i] << RESET << std::endl;
			stbi_image_free(data);
		}
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return m_RendererID;
}
