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
#include "Rays/Ray3D.h"
#include "EntryPoint.h"

using namespace Doom;

DOOM_API std::vector<LuaState*> LuaState::s_LuaStates;

DOOM_API std::vector<CubeCollider3D*> CubeCollider3D::s_Colliders;
DOOM_API std::vector<SphereCollider*> SphereCollider::s_Spheres;

DOOM_API std::vector <SpriteRenderer*> Renderer::s_Objects2d;
DOOM_API std::vector <Renderer3D*> Renderer::s_Objects3d;
DOOM_API std::vector <Renderer3D*> Renderer::s_Objects3dTransparent;

DOOM_API std::vector <RectangleCollider2D*> RectangleCollider2D::s_Collision2d;
DOOM_API Shader* RectangleCollider2D::s_Shader;
DOOM_API Batch* Batch::s_Instance;
DOOM_API bool Renderer::s_PolygonMode = false;
DOOM_API Renderer::Stats Renderer::s_Stats;
DOOM_API float Renderer::s_Exposure = 1.5f;
DOOM_API bool Renderer::s_BloomEffect = true;
DOOM_API float Renderer::s_Brightness = 1.0;

DOOM_API int World::s_ObjId = 0;
DOOM_API int World::s_ColId = 0;

DOOM_API std::vector<std::string> Editor::s_TexturesPath;
DOOM_API std::vector<Texture*> Editor::s_Texture;
DOOM_API std::vector<Texture*> Editor::s_TextureVecTemp;
DOOM_API std::vector<Texture*> Texture::s_LoadedTextures;
DOOM_API bool Texture::s_IsTextureAdded = false;
DOOM_API std::map<void*, std::function<Texture*()>> Texture::s_WaitingForTextures;
DOOM_API std::mutex Texture::s_LockTextureLoadingMtx;
DOOM_API Texture* Texture::s_WhiteTexture;
DOOM_API std::map<std::string, Texture*> Texture::s_Textures;

DOOM_API double Timer::s_OutTime = 0;

DOOM_API ThreadPool* ThreadPool::s_Instance = nullptr;
DOOM_API bool ThreadPool::m_IsInitialized;

DOOM_API Input* Input::s_Instance = new WindowsInput();

DOOM_API GLFWwindow* Window::s_Window = nullptr;
DOOM_API ImGuiIO* Window::s_ImGuiIO = nullptr;
DOOM_API ImGuiContext* Window::s_ImGuiContext = nullptr;
DOOM_API float Window::s_ScrollYOffset = 0;
DOOM_API Camera* Window::s_Camera = new Camera();
DOOM_API bool Window::s_CursorStateChanged = false;

DOOM_API float DeltaTime::s_Time;
DOOM_API float DeltaTime::s_Lasttime = (float)glfwGetTime();
DOOM_API float DeltaTime::s_Deltatime = 1.0 / 1e8;
DOOM_API std::mutex DeltaTime::s_Mtx;

DOOM_API bool RectangleCollider2D::s_IsVisible = false;
DOOM_API std::vector <RectangleCollider2D*> RectangleCollider2D::s_CollidersToInit;

DOOM_API vec4 COLORS::Red(1, 0, 0, 1);
DOOM_API vec4 COLORS::Yellow(1, 1, 0, 1);
DOOM_API vec4 COLORS::Blue(0, 0, 1, 1);
DOOM_API vec4 COLORS::Green(0, 1, 0, 1);
DOOM_API vec4 COLORS::Brown(0.5, 0.3, 0.1, 1);
DOOM_API vec4 COLORS::White(1, 1, 1, 1);
DOOM_API vec4 COLORS::Orange(1, 0.31, 0, 1);
DOOM_API vec4 COLORS::Gray(0.86, 0.86, 0.86, 1);
DOOM_API vec4 COLORS::Silver(0.75, 0.75, 0.75, 1);
DOOM_API vec4 COLORS::DarkGray(0.4, 0.4, 0.4, 1);

DOOM_API std::vector<Line*> Line::s_Lines;
DOOM_API float Line::s_Width = 1.0f;

DOOM_API std::map<std::string, TextureAtlas*> TextureAtlas::s_TextureAtlases;
DOOM_API const char** TextureAtlas::s_NamesOfTextureAtlases;

DOOM_API std::map<std::string,Shader*> Shader::s_Shaders;

DOOM_API std::vector<Particle*> Particle::s_Particles;

DOOM_API ALCdevice* SoundManager::s_AlcDevice = nullptr;
DOOM_API ALCcontext* SoundManager::s_AlcContext = nullptr;
DOOM_API std::map<std::string,Sound*> SoundManager::s_Sounds;

DOOM_API std::map<std::string, Mesh*> MeshManager::s_Meshes;
DOOM_API std::multimap<std::string, void*> MeshManager::s_MeshQueue;
DOOM_API std::vector <Mesh*> MeshManager::s_NeedToInitMeshes;
DOOM_API const char** MeshManager::s_NamesOfMeshes;
DOOM_API std::mutex MeshManager::s_Mtx;
DOOM_API Shader* Font::s_Shader = nullptr;
DOOM_API const char** Shader::s_NamesOfShaders;

DOOM_API std::vector<PointLight*> PointLight::s_PointLights;
DOOM_API std::vector<DirectionalLight*> DirectionalLight::s_DirLights;

//storage of all references to our objects in the scene
DOOM_API std::vector <GameObject*> World::s_GameObjects;

DOOM_API unsigned int SpriteRenderer::s_Indices2D[6] = { 0,1,2,3,2,0 };
DOOM_API float RectangleCollider2D::m_Vertices[8] = {-0.5f, -0.5f, 0.5f, -0.5f, 0.5f,  0.5f, -0.5f,  0.5f };
DOOM_API std::vector<std::string> Ray3D::m_IgnoreMask;

DOOM_API std::string SceneSerializer::s_CurrentSceneFilePath = "src/Scenes/aimtrainer.yaml";

DOOM_API void* World::s_Application = nullptr;