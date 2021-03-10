#include "EntryPoint.h"
#include "Core/Timer.h"
#include "Objects/Line.h"
#include "Core/World.h"
#include "Audio/SoundManager.h"
#include "Core/Editor.h"
#include "Objects/GridLayOut.h"
#include "Objects/SkyBox.h"
#include "Render/MeshManager.h"
#include "Render/Instancing.h"
#include "Core/Utils.h"
#include "Core/SceneSerializer.h"
#include "Lua/LuaState.h"
#include "ImGuizmo/ImGuizmo.h"
#include "ParticleSystem/ParticleEmitter.h"

using namespace Doom;

#define _IS_GAME_BUILD

EntryPoint::EntryPoint(Doom::Application* app)
{
	m_App = app == nullptr ? new Application : app;
	World::GetInstance().s_Application = m_App;

	Window::GetInstance().Init(m_App->m_Name.c_str(), m_App->m_Width, m_App->m_Height, m_App->m_Vsync);
	this->m_App->Init();
	ThreadPool::Init();
	SoundManager::GetInstance().Init();
	MainThread::GetInstance();
	Texture::s_WhiteTexture = Texture::ColoredTexture("WhiteTexture",0xFFFFFFFF);
	Texture::ColoredTexture("InvalidTexture", 0xFF00AC);

	Gui::GetInstance().LoadStandartFonts();
	
	Window& window = Window::GetInstance();
	int* size = Window::GetInstance().GetSize();
	window.m_FrameBufferShadowMap = new FrameBuffer(2056, 2056, GL_DEPTH_COMPONENT, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_DEPTH_ATTACHMENT, false, false, false);
	window.m_FrameBufferColor = new FrameBuffer(size[0], size[1], GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_COLOR_ATTACHMENT0, true, true, true, 2);
	window.m_FrameBufferColor->Bind();
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	window.m_FrameBufferColor->UnBind();
	window.m_FrameBufferBlur.push_back(new FrameBuffer(size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_COLOR_ATTACHMENT0, false, true, true, 1));
	window.m_FrameBufferBlur.push_back(new FrameBuffer(size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_COLOR_ATTACHMENT0, false, true, true, 1));
	if (this->m_App->m_Type == TYPE_3D) 
	{
		GridLayOut::DrawGrid(51, 50);
		//Editor::GetInstance()->gizmo = new Gizmos; @Deprecated
	}

#ifndef LOADFROMFILE
	Utils::LoadShadersFromFolder("src/Shaders");
	Utils::LoadTexturesFromFolder("src/Images");
	Utils::LoadMeshesFromFolder("src/Mesh/Primitives");
#endif

	CubeCollider3D::InitMesh();

	//Pre-reserve vectors
	{
		CubeCollider3D::s_Colliders.reserve(MAX_PREALLOCATED_INSTANCES);
		RectangleCollider2D::s_Collision2d.reserve(MAX_PREALLOCATED_INSTANCES);
		World::GetInstance().s_GameObjects.reserve(MAX_PREALLOCATED_INSTANCES);
		Renderer::s_Objects2d.reserve(MAX_PREALLOCATED_INSTANCES);
		Renderer::s_Objects3d.reserve(MAX_PREALLOCATED_INSTANCES);
		Renderer::s_Objects3dTransparent.reserve(MAX_PREALLOCATED_INSTANCES);
		SphereCollider::s_Spheres.reserve(MAX_PREALLOCATED_INSTANCES);
		PointLight::s_PointLights.reserve(MAX_PREALLOCATED_INSTANCES);
	}

	std::cout << glGetString(GL_VERSION) << std::endl;
}

void EntryPoint::Run()
{
	bool isEditorEnable = false;
	bool FirstFrame = true;
#ifdef _IS_GAME_BUILD
	m_App->OnStart();
#endif
	EventSystem::GetInstance().SendEvent(EventType::ONSTART, nullptr);
	while (!glfwWindowShouldClose(Window::GetInstance().GetWindow())) 
	{
		RectangleCollider2D::CollidersToInit();
		Window::GetInstance().s_CursorStateChanged = false;
		Gui::GetInstance().m_IsAnyPanelHovered = false;
		EventSystem::GetInstance().SendEvent(EventType::ONUPDATE, nullptr);
		DeltaTime::calculateDeltaTime();
		Editor::GetInstance().ShortCuts();

		if (FirstFrame) 
		{
			DeltaTime::s_Deltatime = 0.000001;
			FirstFrame = false;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		Window::GetInstance().ClampCursorPos();
		Window::GetInstance().GetCamera().WindowResize();

//#ifndef _IS_GAME_BUILD
		if (Input::IsKeyPressed(Keycode::KEY_E))
			isEditorEnable = !isEditorEnable;
		if (m_App->m_Type == RenderType::TYPE_3D) 
			World::GetInstance().SelectObject3D();
		if (isEditorEnable)
			Editor::GetInstance().EditorUpdate();
		Window::GetInstance().GetCamera().CameraMovement();
//#endif
		MeshManager::GetInstance().DispatchLoadedMeshes();
		Texture::DispatchLoadedTextures();
		SoundManager::GetInstance().UpdateSourceState();

		World::GetInstance().UpdateLuaStates();
		EventSystem::GetInstance().ProcessEvents();

		if (m_App->m_Type == RenderType::TYPE_3D)
			Renderer::SortTransparentObjects();

		Renderer::UpdateLightSpaceMatrices();

#ifdef _IS_GAME_BUILD
		m_App->OnUpdate();
		Gui::GetInstance().Begin();
		m_App->OnGuiRender();
		Gui::GetInstance().End();
#endif

		Renderer::Render();

		ViewPort::GetInstance()->Update();
		if (ViewPort::GetInstance()->m_IsViewportResized)
		{
			Window& window = Window::GetInstance();
			int* size = Window::GetInstance().GetSize();
			window.m_FrameBufferColor->  Resize(size[0], size[1]);
			window.m_FrameBufferBlur[0]->Resize(size[0], size[1]);
			window.m_FrameBufferBlur[1]->Resize(size[0], size[1]);
		}
		
		try
		{
			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(Window::GetInstance().GetWindow());
			glfwPollEvents();
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}

		
	}
#ifdef _IS_GAME_BUILD
	m_App->OnClose();
#endif
}

EntryPoint::~EntryPoint()
{
	World::GetInstance().ShutDown();
	MeshManager::GetInstance().ShutDown();
	Gui::GetInstance().ShutDown();
	EventSystem::GetInstance().Shutdown();
	ThreadPool::GetInstance().Shutdown();
	Instancing::GetInstance()->ShutDown();
	Texture::ShutDown();
	SoundManager::GetInstance().ShutDown();
	ComponentManager::ShutDown();
	delete &Window::GetInstance().GetCamera();
	
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}