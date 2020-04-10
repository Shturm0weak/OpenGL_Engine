
#include "../Render/Character.h"
#include "Timer.h"
#include "Core.h"
#include "../Render/Renderer.h"

	//storage of all references to our objects in the scene
    DOOM_API std::vector <std::reference_wrapper<Renderer2DLayer>> Renderer2DLayer::objects2d;
	//storage of all references to our object's collisions in the scene
	DOOM_API std::vector <std::reference_wrapper<Renderer2DLayer>> Renderer2DLayer::collision2d;

	DOOM_API  std::vector<Character*> Font::characters;
	DOOM_API const char** Renderer::items;
	DOOM_API std::mutex Renderer::mtx;
	DOOM_API std::condition_variable Renderer::condVar;
	DOOM_API int Renderer::DrawCalls;
	 //visible collisions
	DOOM_API int Renderer2DLayer::obj_id = 0;
	DOOM_API int Renderer2DLayer::col_id = 0;
	 //ThreadPool
	 //Textures
	DOOM_API std::vector<std::string> Editor::texturesPath;
	DOOM_API std::vector<Texture*> Editor::texture;
	DOOM_API int Texture::bindedAmount = 0;
	DOOM_API double Texture::VRAMused = 0;
	DOOM_API std::vector<Texture*> Texture::texturesArray;
	DOOM_API std::map<unsigned int, unsigned int> Texture::Texturesused;
	 //Timer
	DOOM_API double Timer::start;