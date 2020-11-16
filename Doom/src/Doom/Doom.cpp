#include "pch.h"
#include "Core/World.h"
#include "Core/Core.h"
#include "Render/Renderer.h"
#include "Text/Character.h"
#include "Core/Timer.h"
#include "Render/Batch.h"
#include "Objects/Line.h"
#include "Objects/ParticleSystem.h"
#include "Audio/SoundManager.h"
#include "Core/Editor.h"
#include "Components/RectangleCollider2D.h"
#include "Components/PointLight.h"
#include "Lua/LuaState.h"
#include "Core/SceneSerializer.h"
#include "Input/WindowsInput.h"

using namespace Doom;

DOOM_API std::vector<LuaState*> LuaState::luaStates;

DOOM_API std::vector<CubeCollider3D*> CubeCollider3D::colliders;
DOOM_API std::vector<SphereCollider*> SphereCollider::spheres;

DOOM_API std::vector <SpriteRenderer*> Renderer::objects2d;
DOOM_API std::vector <Renderer3D*> Renderer::objects3d;
DOOM_API std::vector <Renderer3D*> Renderer::objects3dTransparent;

DOOM_API std::vector <RectangleCollider2D*> Renderer::collision2d;
DOOM_API Batch* Batch::instance;
DOOM_API std::vector<unsigned int> Renderer::ObjectsWithOwner;
DOOM_API std::vector<unsigned int> Renderer::ObjectsWithNoOwner;
DOOM_API const char** Renderer::items;
DOOM_API std::mutex Renderer::mtx;
DOOM_API std::condition_variable Renderer::condVar;
DOOM_API bool Renderer::PolygonMode = false;
DOOM_API int Renderer::DrawCalls = 0;
DOOM_API int Renderer::Vertices = 0;

DOOM_API int Renderer::obj_id = 0;
DOOM_API int Renderer::col_id = 0;

DOOM_API std::vector<std::string> Editor::texturesPath;
DOOM_API std::vector<Texture*> Editor::texture;
DOOM_API std::vector<Texture*> Editor::textureVecTemp;
DOOM_API int Texture::bindedAmount = 0;
DOOM_API std::vector<Texture*> Texture::loadedTextures;
DOOM_API bool Texture::textureAdded = false;
DOOM_API std::map<void*, std::function<Texture*()>> Texture::waitingForTextures;
DOOM_API std::mutex Texture::lockTextureLoading;
DOOM_API Texture* Texture::WhiteTexture;
DOOM_API double Texture::VRAMused = 0;
DOOM_API std::map<std::string, Texture*> Texture::textures;

DOOM_API double Timer::start;

DOOM_API ThreadPool* ThreadPool::thread_pool = nullptr;
DOOM_API bool ThreadPool::initialized;

DOOM_API Input* Input::s_Instance = new WindowsInput();

DOOM_API GLFWwindow* Window::m_window = nullptr;
DOOM_API ImGuiIO* Window::io = nullptr;
DOOM_API ImGuiContext* Window::imGuiContext = nullptr;
DOOM_API float Window::scrollYOffset = 0;
DOOM_API Camera* Window::m_camera = new Camera();

DOOM_API float DeltaTime::time;
DOOM_API float DeltaTime::lasttime = (float)glfwGetTime();
DOOM_API float DeltaTime::deltatime = 0.0000001;
DOOM_API std::mutex DeltaTime::mtx;

DOOM_API bool RectangleCollider2D::IsVisible;

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

DOOM_API std::vector<Line*> Line::lines;
DOOM_API float Line::width = 1.0f;

DOOM_API std::map<std::string, TextureAtlas*> TextureAtlas::textureAtlases;
DOOM_API const char** TextureAtlas::items;

DOOM_API std::map<std::string,Shader*> Shader::shaders;

DOOM_API std::vector<Particle*> Particle::particles;

DOOM_API ALCdevice* SoundManager::alcDevice = nullptr;
DOOM_API ALCcontext* SoundManager::alcContext = nullptr;
DOOM_API std::map<std::string,Sound*> SoundManager::sounds;

DOOM_API std::map<std::string, Mesh*> MeshManager::Meshes;
DOOM_API std::multimap<std::string, void*> MeshManager::meshQueue;
DOOM_API std::vector <Mesh*> MeshManager::needToInitMeshes;
DOOM_API const char** MeshManager::listOfMeshes;
DOOM_API Shader* Font::shader = nullptr;
DOOM_API const char** Shader::listOfShaders;

DOOM_API std::vector<PointLight*> PointLight::pLights;
DOOM_API std::vector<DirectionalLight*> DirectionalLight::dirLights;

//storage of all references to our objects in the scene
DOOM_API std::vector <GameObject*> World::objects;

DOOM_API unsigned int Character::indeces2D[6] = { 0,1,2,3,2,0 };
DOOM_API unsigned int SpriteRenderer::indeces2D[6] = { 0,1,2,3,2,0 };

DOOM_API std::string SceneSerializer::currentSceneFilePath = "src/Scenes/scene.yaml";