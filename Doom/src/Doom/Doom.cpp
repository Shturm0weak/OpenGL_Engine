#include "pch.h"
#include "Core.h"
#include "Render/Renderer.h"
#include "Render/Character.h"
#include "Core/Timer.h"
#include "Render/Batch.h"
#include "Render/Line.h"

using namespace Doom;

//storage of all references to our objects in the scene
DOOM_API std::vector <std::reference_wrapper<Renderer2DLayer>> Renderer2DLayer::objects2d;
//storage of all references to our object's collisions in the scene
DOOM_API std::vector <std::reference_wrapper<Renderer2DLayer>> Renderer2DLayer::collision2d;
DOOM_API Batch* Batch::instance;
DOOM_API  std::vector<Character*> Font::characters;
DOOM_API std::vector<unsigned int> Renderer::ObjectsWithOwner;
DOOM_API std::vector<unsigned int> Renderer::ObjectsWithNoOwner;
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
//ThreadPool
DOOM_API ThreadPool* ThreadPool::thread_pool = nullptr;
DOOM_API bool ThreadPool::initialized;

DOOM_API GLFWwindow* Window::m_window = nullptr;
DOOM_API OrthographicCamera Window::m_camera(-16.f, 16.f, 9.f, -9.f);

DOOM_API float DeltaTime::time;
DOOM_API float DeltaTime::lasttime;
DOOM_API float DeltaTime::deltatime;
DOOM_API std::mutex DeltaTime::mtx;

DOOM_API bool Collision::IsVisible;

DOOM_API vec4 COLORS::Red(0.9, 0, 0, 1);
DOOM_API vec4 COLORS::Yellow(1, 1, 0, 1);
DOOM_API vec4 COLORS::Blue(0, 0, 1, 1);
DOOM_API vec4 COLORS::Green(0, 1, 0, 1);
DOOM_API vec4 COLORS::Brown(0.5, 0.3, 0.1, 1);
DOOM_API vec4 COLORS::White(1, 1, 1, 1);
DOOM_API vec4 COLORS::Orange(1, 0.31, 0, 1);

DOOM_API bool Renderer::isReadyToRenderFirstThread = false;
DOOM_API bool Renderer::isReadyToRenderSecondThread = false;
DOOM_API bool Renderer::isReadyToRenderThirdThread = false;
DOOM_API std::vector<Line*> Line::lines;