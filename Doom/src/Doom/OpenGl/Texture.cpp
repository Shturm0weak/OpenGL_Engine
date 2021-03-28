#include "../pch.h"
#include "Texture.h"
#include "stb_image.h"
#include <filesystem>
namespace fs = std::filesystem;

using namespace Doom;

Texture::Texture(const std::string& path, int flip,bool repeat)
	:m_RendererID(0), m_FilePath(path), m_LocalBuffer(nullptr),
	m_height(0), m_width(0), m_BPP(0)
{
	auto iter = s_Textures.find(path);
	if(iter == s_Textures.end())
		s_Textures.insert(std::make_pair(path, this));
	else 
	{
#ifdef _DEBUG
		std::cout << NAMECOLOR << "Texture" << BOLDYELLOW << ": <" << NAMECOLOR << path << BOLDYELLOW << "> has already existed\n" << RESET;
#endif
		return;
	}
	stbi_set_flip_vertically_on_load(flip);
	m_LocalBuffer = stbi_load(path.c_str(), &m_width, &m_height, &m_BPP, 4);
	
	glGenTextures(1, &m_RendererID);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	if (repeat) 
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_REPEAT);
	}
	else 
	{
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
	}
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_width, m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_LocalBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);
	if (m_LocalBuffer)
		UnloadFromRAM(m_FilePath);
	s_IsTextureAdded = true;
}

Texture::Texture()
{
}

std::vector<Texture*> Texture::GetLoadedTexturesFromFolder(std::string filePath) 
{
	std::vector<Texture*> ts;
	for (const auto& entry : fs::directory_iterator(filePath)) 
	{
		std::string pathToTexture = entry.path().string();
		if (pathToTexture.find(".png") <= pathToTexture.length() || pathToTexture.find(".jpeg") <= pathToTexture.length())
		{
			size_t index = 0;
			index = pathToTexture.find("\\", index);
			pathToTexture.replace(index, 1, "/");
			Texture* t = Texture::Get(pathToTexture);
			if (t != nullptr)
				ts.push_back(t);
		}
	}
	return ts;
}

void Doom::Texture::DispatchLoadedTextures()
{
	std::lock_guard<std::mutex> lock(s_LockTextureLoadingMtx);
	for (uint32_t i = 0; i < s_LoadedTextures.size(); i++)
	{
		LoadTextureInVRAM(s_LoadedTextures[i]->m_FilePath,true);
	}
	s_LoadedTextures.clear();
	if (s_IsTextureAdded) 
	{
		s_IsTextureAdded = false;
		for (auto i = s_WaitingForTextures.begin(); i != s_WaitingForTextures.end();)
		{
			std::function<Texture*()> f = i->second;
			Texture* t = f();
			if (t != nullptr)
				s_WaitingForTextures.erase(i++);
			else
				++i;
		}
	}
}

Texture::~Texture() 
{
	auto it = s_Textures.find(m_FilePath);
	if (it != s_Textures.end())
	{
		s_Textures.erase(it);
		UnloadFromRAM(m_FilePath);
		UnloadFromVRAM(m_FilePath);
	}
}

void Texture::Bind(unsigned int slot) const 
{
	glActiveTexture(GL_TEXTURE0 + slot);
	glBindTexture(GL_TEXTURE_2D, m_RendererID);
}

void Texture::UnBind() const
{
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Doom::Texture::ShutDown()
{
	std::vector<Texture*> ts;
	for (auto i = s_Textures.begin(); i != s_Textures.end(); i++)
	{
		ts.push_back(i->second);
	}
	s_Textures.clear();
	for (uint32_t i = 0; i < ts.size(); i)
	{
		delete ts[i];
		ts.erase(ts.begin() + i);
	}
}

void Doom::Texture::Delete(Texture* texture)
{
	auto iter = s_Textures.find(texture->m_FilePath);
	if (iter != s_Textures.end()) {
		delete iter->second;
		s_Textures.erase(iter);
	}
}

void Doom::Texture::AsyncLoadTexture(const std::string & filePath)
{
	ThreadPool::GetInstance().Enqueue([=] {
		LoadTextureInRAM(filePath, true);
		{
			std::lock_guard<std::mutex> lock(s_LockTextureLoadingMtx);
			s_LoadedTextures.push_back(Texture::Get(filePath));
		}
	});
}

Texture* Doom::Texture::Get(std::string filePath,bool showErrors)
{
	auto iter = s_Textures.find(filePath);
	if (iter != s_Textures.end()) return iter->second;
	else 
	{
		if(showErrors)
			std::cout << NAMECOLOR << "Texture" << BOLDYELLOW << ": <" << NAMECOLOR << filePath << BOLDYELLOW << "> doesn't exist\n" << RESET;
		return nullptr;
	}
}

void Texture::GetAsync(void* ptr, std::function<Texture*()> f) 
{
	s_WaitingForTextures.insert(std::make_pair(ptr, f));
}

void Doom::Texture::RemoveFromGetAsync(void * ptr)
{
	auto iter = s_WaitingForTextures.find(ptr);
	if (iter != s_WaitingForTextures.end())
		s_WaitingForTextures.erase(iter);
}

bool Doom::Texture::UnloadFromRAM(const std::string& filePath)
{
	Texture* t = Get(filePath,false);
	if (t != nullptr && t->m_LocalBuffer)
	{
		stbi_image_free(t->m_LocalBuffer);
		t->m_LocalBuffer = nullptr;
		return true;
	}
	return false;
}

bool Doom::Texture::UnloadFromVRAM(const std::string& filePath)
{
	Texture* t = Get(filePath,false);
	if (t != nullptr || t != NULL)
	{
		glDeleteTextures(1, &t->m_RendererID);
		t->m_RendererID = -1;
		return true;
	}
	return false;
}

Texture * Doom::Texture::ColoredTexture(const std::string& name, uint32_t color)
{
	auto iter = s_Textures.find(name);
	if (iter != s_Textures.end()) 
	{
#ifdef _DEBUG
		std::cout << NAMECOLOR << "Texture" << BOLDYELLOW << ": <" << NAMECOLOR << name << BOLDYELLOW << "> has already existed\n" << RESET;
#endif
		return iter->second;
	}
	Texture* t = new Texture();
	s_IsTextureAdded = true;
	t->m_FilePath = name;
	glGenTextures(1, &t->m_RendererID);
	glBindTexture(GL_TEXTURE_2D, t->m_RendererID);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &color);
	glBindTexture(GL_TEXTURE_2D, 0);
	s_Textures.insert(std::make_pair(t->m_FilePath, t));
	return t;
}

Texture * Doom::Texture::Create(const std::string& filePath,bool flip,bool repeat)
{
	Texture* t = Get(filePath,false);
	if (t == nullptr || t == NULL) return new Texture(filePath, flip, repeat);
	else return t;
}

bool Doom::Texture::LoadTextureInRAM(const std::string& filePath, bool flip)
{
	Texture* t = nullptr;
	auto iter = s_Textures.find(filePath);
	if (iter == s_Textures.end())
	{
		t = new Texture();
		s_IsTextureAdded = true;
		t->m_FilePath = filePath;
		std::lock_guard<std::mutex> lock(Texture::s_LockTextureLoadingMtx);
		s_Textures.insert(std::make_pair(t->m_FilePath, t));
	}
	else
		t = iter->second;
	if (t->m_LocalBuffer == nullptr)
	{
		stbi_set_flip_vertically_on_load(flip);
		t->m_LocalBuffer = stbi_load(t->m_FilePath.c_str(), &t->m_width, &t->m_height, &t->m_BPP, 4);
		return true;
	}
	else
	{
#ifdef _DEBUG
		std::cout << NAMECOLOR << "Texture" << BOLDYELLOW << ": <" << NAMECOLOR << filePath << BOLDYELLOW << "> m_localBuffer is not empty\n" << RESET;
#endif
		return false;
	}
}

bool Doom::Texture::LoadTextureInVRAM(const std::string& filePath, bool unloadFromRam)
{
	Texture* t = Get(filePath);
	if (t == nullptr) return false;
	if (t->m_RendererID != -1) 
	{
#ifdef _DEBUG
		std::cout << NAMECOLOR << "Texture" << BOLDYELLOW << ": <" << NAMECOLOR << filePath << BOLDYELLOW << "> has been already in VRAM\n" << RESET;
#endif
		//UnloadFromVRAM(filePath);
		return true;
	}
	if(t->m_LocalBuffer == nullptr)
	{
#ifdef _DEBUG
		std::cout << NAMECOLOR << "Texture" << RED ": <"<< NAMECOLOR << filePath << RED << "> Can't be loaded in VRAM, m_LocalBuffer is unloaded from RAM!\n" << RESET;
#endif
		return false;
	}

	glGenTextures(1, &t->m_RendererID);
	glBindTexture(GL_TEXTURE_2D, t->m_RendererID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, t->m_width, t->m_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, t->m_LocalBuffer);
	glBindTexture(GL_TEXTURE_2D, 0);
	if (unloadFromRam)
		UnloadFromRAM(t->m_FilePath);
	return true;
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