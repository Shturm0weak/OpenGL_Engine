#include "pch.h"
#include "Core/Core.h"
#include "Render/Renderer.h"
#include "Render/Character.h"
#include "Core/Timer.h"
#include "Render/Batch.h"
#include "Render/Line.h"
#include "Render/ParticleSystem.h"
#include "Audio/SoundManager.h"
#include "Core/Editor.h"
#include "Components/Collision.h"

using namespace Doom;

//storage of all references to our objects in the scene
DOOM_API std::vector <GameObject*> Renderer::objects2d;
//storage of all references to our object's collisions in the scene
DOOM_API std::vector <Collision*> Renderer::collision2d;
DOOM_API Batch* Batch::instance;
DOOM_API std::vector<unsigned int> Renderer::ObjectsWithOwner;
DOOM_API std::vector<unsigned int> Renderer::ObjectsWithNoOwner;
DOOM_API const char** Renderer::items;
DOOM_API std::mutex Renderer::mtx;
DOOM_API std::condition_variable Renderer::condVar;
DOOM_API int Renderer::DrawCalls;
//visible collisions
DOOM_API int Renderer::obj_id = 0;
DOOM_API int Renderer::col_id = 0;
//ThreadPool
//Textures
DOOM_API std::vector<std::string> Editor::texturesPath;
DOOM_API std::vector<Texture*> Editor::texture;
DOOM_API std::vector<Texture*> Editor::textureVecTemp;
DOOM_API int Texture::bindedAmount = 0;
DOOM_API Texture* Texture::WhiteTexture = new Texture();
DOOM_API double Texture::VRAMused = 0;
DOOM_API std::vector<Texture*> Texture::texturesArray;
DOOM_API std::map<unsigned int, unsigned int> Texture::Texturesused;
//Timer
DOOM_API double Timer::start;
//ThreadPool
DOOM_API ThreadPool* ThreadPool::thread_pool = nullptr;
DOOM_API bool ThreadPool::initialized;

DOOM_API GLFWwindow* Window::m_window = nullptr;
DOOM_API ImGuiIO* Window::io = nullptr;
DOOM_API ImGuiContext* Window::imGuiContext = nullptr;
DOOM_API Camera Window::m_camera(-16.f, 16.f, 9.f, -9.f,-1,10);

DOOM_API float DeltaTime::time;
DOOM_API float DeltaTime::lasttime = (float)glfwGetTime();
DOOM_API float DeltaTime::deltatime = 0.0000001;
DOOM_API std::mutex DeltaTime::mtx;

DOOM_API bool Collision::IsVisible;

DOOM_API vec4 COLORS::Red(0.9, 0, 0, 1);
DOOM_API vec4 COLORS::Yellow(1, 1, 0, 1);
DOOM_API vec4 COLORS::Blue(0, 0, 1, 1);
DOOM_API vec4 COLORS::Green(0, 1, 0, 1);
DOOM_API vec4 COLORS::Brown(0.5, 0.3, 0.1, 1);
DOOM_API vec4 COLORS::White(1, 1, 1, 1);
DOOM_API vec4 COLORS::Orange(1, 0.31, 0, 1);
DOOM_API vec4 COLORS::Gray(0.86, 0.86, 0.86, 1);
DOOM_API vec4 COLORS::Silver(0.75, 0.75, 0.75, 1);
DOOM_API vec4 COLORS::DarkGray(0.4, 0.4, 0.4, 1);


DOOM_API bool Renderer::isReadyToRenderFirstThread = false;
DOOM_API bool Renderer::isReadyToRenderSecondThread = false;
DOOM_API bool Renderer::isReadyToRenderThirdThread = false;
DOOM_API std::vector<Line*> Line::lines;
DOOM_API float Line::width = 1.0f;

DOOM_API std::vector<TextureAtlas*> TextureAtlas::textureAtlases;
DOOM_API const char** TextureAtlas::items;

DOOM_API Shader* Shader::defaultShader = nullptr;
DOOM_API std::vector<Shader*> Shader::shaders;

DOOM_API std::vector<Particle*> Particle::particles;

DOOM_API ALCdevice* SoundManager::alcDevice = nullptr;
DOOM_API ALCcontext* SoundManager::alcContext = nullptr;
DOOM_API std::map<std::string,Sound*> SoundManager::sounds;