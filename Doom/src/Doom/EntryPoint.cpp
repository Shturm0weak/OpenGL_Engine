#include "pch.h"
#include "EntryPoint.h"
#include "Core/Timer.h"
#include "Objects/Line.h"
#include "Core/World.h"
#include "Core/Editor.h"
#include "Objects/GridLayOut.h"
#include "Objects/SkyBox.h"
#include "Render/Instancing.h"
#include "Core/Utils.h"
#include "Core/SceneSerializer.h"
#include "Lua/LuaState.h"
#include "ImGuizmo/ImGuizmo.h"
#include "Components/ParticleEmitter.h"

using namespace Doom;

#define _IS_GAME_BUILD

EntryPoint::EntryPoint(Doom::Application* app)
{
	Logger::UpdateTime();

	Window& window = Window::GetInstance();

	window.Init(app);
	ThreadPool::Init();
	SoundManager::GetInstance().Init();
	MainThread::GetInstance();
	Input::SetupCallBack();

#ifndef LOADFROMFILE
	Utils::LoadShadersFromFolder("src/Shaders");
	Utils::LoadTexturesFromFolder("src/Images");
	Utils::LoadTexturesFromFolder("src/UIimages");
	Utils::LoadMeshesFromFolder("src/Mesh/Primitives");
#endif
	Utils::SetStandardTexParams();
	Texture::s_WhiteTexture = Texture::ColoredTexture("WhiteTexture",0xFFFFFFFF);
	Texture::ColoredTexture("InvalidTexture", 0xFF00AC);

	Gui::GetInstance().LoadStandartFonts();
	
	int* size = Window::GetInstance().GetSize();
	Doom::Texture::s_TexParameters[2] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER };
	Doom::Texture::s_TexParameters[3] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER };
	FrameBufferParams shadowMapParams = { 4096, 4096, GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT, false, false, false };
	window.m_FrameBufferShadowMap = new FrameBuffer(shadowMapParams);

	FrameBufferParams outlineParams = { (uint32_t)size[0], (uint32_t)size[1], GL_DEPTH_ATTACHMENT, GL_DEPTH_COMPONENT, GL_FLOAT, false, false, false };
	window.m_FrameBufferOutline = new FrameBuffer(outlineParams);

	Doom::Texture::s_TexParameters[2] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE };
	Doom::Texture::s_TexParameters[3] = { GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE };
	FrameBufferParams colorParams = { (uint32)size[0], (uint32)size[1], GL_COLOR_ATTACHMENT0, GL_RGBA , GL_UNSIGNED_BYTE, true, true, true, 2 };
	window.m_FrameBufferColor = new FrameBuffer(colorParams);
	window.m_FrameBufferColor->Bind();
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	window.m_FrameBufferColor->UnBind();

	colorParams = { (uint32)size[0], (uint32)size[1], GL_COLOR_ATTACHMENT0, GL_RGBA , GL_UNSIGNED_BYTE, true, true, true, 1 };
	window.m_FrameBufferMainImage = new FrameBuffer(colorParams);

	FrameBufferParams Blur1Params = { (uint32)size[0], (uint32)size[1], GL_COLOR_ATTACHMENT0, GL_RGBA, GL_UNSIGNED_BYTE, false, true, true };
	window.m_FrameBufferBlur.push_back(new FrameBuffer(Blur1Params));
	window.m_FrameBufferBlur.push_back(new FrameBuffer(Blur1Params));

	Utils::SetStandardTexParams();

	//GridLayOut::DrawGrid(51, 50);
	//Editor::GetInstance()->gizmo = new Gizmos; @Deprecated

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
	Window& window = Window::GetInstance();
	EventSystem& eventSystem = EventSystem::GetInstance();
	ViewPort& viewPort = ViewPort::GetInstance();
	Gui& gui = Gui::GetInstance();
	World& world = World::GetInstance();

	bool isEditorEnable = false;
	bool FirstFrame = true;
#ifdef _IS_GAME_BUILD
	window.s_Application->OnStart();
#endif
	eventSystem.SendEvent(EventType::ONSTART, nullptr);
	while (!glfwWindowShouldClose(window.GetWindow())) 
	{
		Logger::UpdateTime();
		Renderer::s_OutLined3dObjects.clear();
		window.s_CursorStateChanged = false;
		eventSystem.SendEvent(EventType::ONUPDATE, nullptr);
		DeltaTime::calculateDeltaTime();

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

		window.ClampCursorPos();

//#ifndef _IS_GAME_BUILD
		if (Input::IsKeyPressed(Keycode::KEY_E))
			isEditorEnable = !isEditorEnable;
		world.SelectObject3D();
		if (isEditorEnable)
			Editor::GetInstance().EditorUpdate();
		window.GetCamera().CameraMovement();
		Editor::GetInstance().ShortCuts();
//#endif
		SoundManager::GetInstance().UpdateSourceState();

		world.UpdateLuaStates();
		eventSystem.ProcessEvents();

		Renderer::SortTransparentObjects();
		Renderer::UpdateLightSpaceMatrices();

#ifdef _IS_GAME_BUILD
		window.s_Application->OnUpdate();
		gui.m_IsAnyPanelHovered = false;
		gui.Begin();
		window.s_Application->OnGuiRender();
		gui.End();
#endif

		Renderer::Render();

		viewPort.Update();
		viewPort.Resize();

		try
		{
			ImGui::EndFrame();
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
			glfwSwapBuffers(window.GetWindow());
			Input::ResetInput();
			glfwPollEvents();
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}

		
	}
#ifdef _IS_GAME_BUILD
	window.s_Application->OnClose();
#endif
}

EntryPoint::~EntryPoint()
{
	World::GetInstance().ShutDown();
	Mesh::ShutDown();
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