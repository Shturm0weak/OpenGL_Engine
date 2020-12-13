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

#define _IS_GAME_BUILD

using namespace Doom;

EntryPoint::EntryPoint(Doom::Application* app) {
	if (app == nullptr)
		this->m_App = new Application();
	else
		this->m_App = app;
	Window::Init(this->m_App->m_Name.c_str(), this->m_App->m_Width, this->m_App->m_Height, this->m_App->m_Vsync);
	this->m_App->Init();
	MainThread::Init();
	ThreadPool::Init();
	Texture::s_WhiteTexture = Texture::ColoredTexture("WhiteTexture",0xFFFFFFFF);
	Texture::ColoredTexture("InvalidTexture", 0xFF00AC);
	CubeCollider3D::InitMesh();

#ifndef LOADFROMFILE
	Utils::LoadShadersFromFolder("src/Shaders");
	Utils::LoadTexturesFromFolder("src/Images");
	Utils::LoadMeshesFromFolder("src/Mesh/Primitives");
#endif

	Gui::GetInstance()->LoadStandartFonts();
	Batch::Init();
	SoundManager::Init();
	Window::GetCamera().m_FrameBufferColor = new FrameBuffer(Window::GetSize()[0], Window::GetSize()[1], GL_RGB,GL_UNSIGNED_BYTE,false, GL_COLOR_ATTACHMENT0,true,true,true);
	Window::GetCamera().m_FrameBufferShadowMap = new FrameBuffer(8192, 8192, GL_DEPTH_COMPONENT, GL_FLOAT, true, GL_DEPTH_ATTACHMENT, false, false, false);
	if (this->m_App->m_Type == TYPE_3D) {
		//GridLayOut* grid = new GridLayOut();
		//Editor::GetInstance()->gizmo = new Gizmos; @Deprecated
	}
}

void EntryPoint::Run()
{
	bool isEditorEnable = false;
	bool FirstFrame = true;
	SceneSerializer::DeSerialize(SceneSerializer::s_CurrentSceneFilePath);
	m_App->OnStart();
	EventSystem::GetInstance()->SendEvent(EventType::ONSTART, nullptr);

	while (!glfwWindowShouldClose(Window::GetWindow())) {
		Window::s_CursorStateChanged = false;
		Gui::GetInstance()->m_IsAnyPanelHovered = false;
		EventSystem::GetInstance()->SendEvent(EventType::ONUPDATE, nullptr);
		//ThreadPool::GetInstance()->Enqueue([] {Editor::GetInstance()->UpdateNormals(); });
		DeltaTime::calculateDeltaTime();
		Editor::GetInstance()->ShortCuts();

		if (FirstFrame) {
			DeltaTime::s_Deltatime = 0.000001;
			FirstFrame = false;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		Window::ClampCursorPos();
		Window::GetCamera().WindowResize();

#ifndef _IS_GAME_BUILD
		Window::GetCamera().CameraMovement();

		if (Input::IsKeyPressed(Keycode::KEY_E)) {
			isEditorEnable = !isEditorEnable;
		}
		if (m_App->m_Type == RenderType::TYPE_3D) {
			World::SelectObject3D();
		}
		if (isEditorEnable) {
			Editor::GetInstance()->EditorUpdate();
		}
#endif
		MeshManager::DispatchLoadedMeshes();
		Texture::DispatchLoadedTextures();
		SoundManager::UpdateSourceState();

		World::ProccessLuaStates();
		EventSystem::GetInstance()->ProcessEvents();
		if (m_App->m_Type == RenderType::TYPE_3D) {
			Renderer::SortTransparentObjects();
		}

		Renderer::UpdateLightSpaceMatrices();
		m_App->OnUpdate();
		Gui::GetInstance()->Begin();
		m_App->OnGuiRender();
		Gui::GetInstance()->End();

		Renderer::s_DrawCalls = 0;
		Renderer::s_Vertices = 0;

		if (Instancing::Instance()->m_DrawShadows > 0.5f) {
			FrameBuffer* shadowMap = Window::GetCamera().m_FrameBufferShadowMap;
			glfwGetWindowSize(Window::GetWindow(), &m_Size[0], &m_Size[1]);
			glViewport(0, 0, shadowMap->size.x, shadowMap->size.y);
			shadowMap->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);
			Renderer::BakeShadows();
			Instancing::Instance()->BakeShadows();
			shadowMap->UnBind();
		}

		glViewport(0, 0, m_Size[0], m_Size[1]);
		glBindFramebuffer(GL_FRAMEBUFFER, Window::GetCamera().m_FrameBufferColor->m_Fbo);
		Renderer::Clear();
		Renderer::Render();
		Window::GetScrollYOffset() = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Renderer::Clear();
		Instancing::Instance()->PrepareVertexAtrrib();

		ViewPort::GetInstance()->Update();

		if (ViewPort::GetInstance()->m_IsViewportResized) {
			Window::GetCamera().m_FrameBufferColor->Resize(Window::GetSize()[0], Window::GetSize()[1]);
		}
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(Window::GetWindow());
		glfwPollEvents();
	}
	SceneSerializer::Serialize(SceneSerializer::s_CurrentSceneFilePath);
	m_App->OnClose();
}

EntryPoint::~EntryPoint() {
	World::ShutDown();
	MeshManager::ShutDown();
	Gui::GetInstance()->ShutDown();
	EventSystem::GetInstance()->Shutdown();
	ThreadPool::GetInstance()->Shutdown();
	Texture::ShutDown();
	SoundManager::ShutDown();
	
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}