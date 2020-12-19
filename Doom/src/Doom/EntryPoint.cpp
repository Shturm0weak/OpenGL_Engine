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
	World::s_Application = this->m_App;
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
	Window::GetCamera().m_FrameBufferShadowMap = new FrameBuffer(2056, 2056, GL_DEPTH_COMPONENT, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_DEPTH_ATTACHMENT, false, false, false);
	Window::GetCamera().m_FrameBufferColor = new FrameBuffer(Window::GetSize()[0], Window::GetSize()[1], GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_COLOR_ATTACHMENT0, true, true, true, 2);
	Window::GetCamera().m_FrameBufferColor->Bind();
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	Window::GetCamera().m_FrameBufferColor->UnBind();
	Window::GetCamera().m_FrameBufferUI = new FrameBuffer(Window::GetSize()[0], Window::GetSize()[1], GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_COLOR_ATTACHMENT0, true, true, true, 2);
	Window::GetCamera().m_FrameBufferBlur.push_back(new FrameBuffer(Window::GetSize()[0], Window::GetSize()[1], GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_COLOR_ATTACHMENT0, false, true, true, 1));
	Window::GetCamera().m_FrameBufferBlur.push_back(new FrameBuffer(Window::GetSize()[0], Window::GetSize()[1], GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_COLOR_ATTACHMENT0, false, true, true, 1));
	if (this->m_App->m_Type == TYPE_3D) {
		//GridLayOut* grid = new GridLayOut();
		//Editor::GetInstance()->gizmo = new Gizmos; @Deprecated
	}
}

void EntryPoint::Run()
{
	bool isEditorEnable = false;
	bool FirstFrame = true;
#ifdef _IS_GAME_BUILD
	m_App->OnStart();
#endif
	EventSystem::GetInstance()->SendEvent(EventType::ONSTART, nullptr);
	while (!glfwWindowShouldClose(Window::GetWindow())) {
		RectangleCollider2D::CollidersToInit();
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
		if (Input::IsKeyPressed(Keycode::KEY_E)) {
			isEditorEnable = !isEditorEnable;
		}
		if (m_App->m_Type == RenderType::TYPE_3D) {
			World::SelectObject3D();
		}
		if (isEditorEnable) {
			Editor::GetInstance()->EditorUpdate();
		}
		Window::GetCamera().CameraMovement();
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

#ifdef _IS_GAME_BUILD
		m_App->OnUpdate();
		Gui::GetInstance()->Begin();
		m_App->OnGuiRender();
		Gui::GetInstance()->End();
#endif

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

		Renderer::RenderBloomEffect();
	
		Shader* shader = Shader::Get("QuadFullScreen");
		Renderer::Clear();
		glBindFramebuffer(GL_FRAMEBUFFER, Window::GetCamera().m_FrameBufferColor->m_Fbo);
		shader->Bind();
		glBindTexture(GL_TEXTURE_2D, Window::GetCamera().m_FrameBufferColor->m_Textures[0]);
		Renderer::RenderForPostEffect(MeshManager::GetMesh("plane"), shader);
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_DEPTH_TEST);
		Renderer::RenderCollision();
		Renderer::RenderCollision3D();
		Renderer::RenderText();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		shader->UnBind();

		ViewPort::GetInstance()->Update();
		if (ViewPort::GetInstance()->m_IsViewportResized) {
			Window::GetCamera().m_FrameBufferColor->Resize(Window::GetSize()[0], Window::GetSize()[1]);
			Window::GetCamera().m_FrameBufferBlur[0]->Resize(Window::GetSize()[0], Window::GetSize()[1]);
			Window::GetCamera().m_FrameBufferBlur[1]->Resize(Window::GetSize()[0], Window::GetSize()[1]);
		}
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(Window::GetWindow());
		glfwPollEvents();
	}
#ifdef _IS_GAME_BUILD
	m_App->OnClose();
#endif
}

EntryPoint::~EntryPoint() {
	World::ShutDown();
	MeshManager::ShutDown();
	Gui::GetInstance()->ShutDown();
	EventSystem::GetInstance()->Shutdown();
	ThreadPool::GetInstance()->Shutdown();
	Texture::ShutDown();
	SoundManager::ShutDown();
	delete &Window::GetCamera();
	
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}