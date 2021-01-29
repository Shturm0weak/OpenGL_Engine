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

using namespace Doom;

#define _IS_GAME_BUILD

EntryPoint::EntryPoint(Doom::Application* app) {
	if (app == nullptr)
		this->m_App = new Application();
	else
		this->m_App = app;
	World::GetInstance().s_Application = this->m_App;
	Window::GetInstance().Init(this->m_App->m_Name.c_str(), this->m_App->m_Width, this->m_App->m_Height, this->m_App->m_Vsync);
	this->m_App->Init();
	//MainThread::GetInstance();
	ThreadPool::Init();
	Texture::s_WhiteTexture = Texture::ColoredTexture("WhiteTexture",0xFFFFFFFF);
	Texture::ColoredTexture("InvalidTexture", 0xFF00AC);
	CubeCollider3D::InitMesh();

#ifndef LOADFROMFILE
	Utils::LoadShadersFromFolder("src/Shaders");
	Utils::LoadTexturesFromFolder("src/Images");
	Utils::LoadMeshesFromFolder("src/Mesh/Primitives");
#endif

	Gui::GetInstance().LoadStandartFonts();
	SoundManager::GetInstance().Init();
	Camera& camera = Window::GetInstance().GetCamera();
	int* size = Window::GetInstance().GetSize();
	camera.m_FrameBufferShadowMap = new FrameBuffer(2056, 2056, GL_DEPTH_COMPONENT, GL_FLOAT, GL_LINEAR, GL_CLAMP_TO_BORDER, GL_DEPTH_ATTACHMENT, false, false, false);
	camera.m_FrameBufferColor = new FrameBuffer(size[0], size[1], GL_RGB, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_COLOR_ATTACHMENT0, true, true, true, 2);
	camera.m_FrameBufferColor->Bind();
	unsigned int attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
	glDrawBuffers(2, attachments);
	camera.m_FrameBufferColor->UnBind();
	camera.m_FrameBufferBlur.push_back(new FrameBuffer(size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_COLOR_ATTACHMENT0, false, true, true, 1));
	camera.m_FrameBufferBlur.push_back(new FrameBuffer(size[0], size[1], GL_RGBA, GL_UNSIGNED_BYTE, GL_LINEAR, GL_CLAMP_TO_EDGE, GL_COLOR_ATTACHMENT0, false, true, true, 1));
	if (this->m_App->m_Type == TYPE_3D) {
		//GridLayOut* grid = new GridLayOut();
		//Editor::GetInstance()->gizmo = new Gizmos; @Deprecated
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
	while (!glfwWindowShouldClose(Window::GetInstance().GetWindow())) {
		RectangleCollider2D::CollidersToInit();
		Window::GetInstance().s_CursorStateChanged = false;
		Gui::GetInstance().m_IsAnyPanelHovered = false;
		EventSystem::GetInstance().SendEvent(EventType::ONUPDATE, nullptr);
		//ThreadPool::GetInstance()->Enqueue([] {Editor::GetInstance()->UpdateNormals(); });
		DeltaTime::calculateDeltaTime();
		Editor::GetInstance().ShortCuts();

		if (FirstFrame) {
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
		if (Input::IsKeyPressed(Keycode::KEY_E)) {
			isEditorEnable = !isEditorEnable;
		}
		if (m_App->m_Type == RenderType::TYPE_3D) {
			World::GetInstance().SelectObject3D();
		}
		if (isEditorEnable) {
			Editor::GetInstance().EditorUpdate();
		}
		Window::GetInstance().GetCamera().CameraMovement();
//#endif
		MeshManager::GetInstance().DispatchLoadedMeshes();
		Texture::DispatchLoadedTextures();
		SoundManager::GetInstance().UpdateSourceState();

		World::GetInstance().ProccessLuaStates();
		EventSystem::GetInstance().ProcessEvents();

		if (m_App->m_Type == RenderType::TYPE_3D) {
			Renderer::SortTransparentObjects();
		}

		Renderer::UpdateLightSpaceMatrices();

#ifdef _IS_GAME_BUILD
		m_App->OnUpdate();
		Gui::GetInstance().Begin();
		m_App->OnGuiRender();
		Gui::GetInstance().End();
#endif

		Renderer::s_Stats.Reset();
		if (Instancing::GetInstance()->m_DrawShadows > 0.5f) {
			Timer t;
			FrameBuffer* shadowMap = Window::GetInstance().GetCamera().m_FrameBufferShadowMap;
			glfwGetWindowSize(Window::GetInstance().GetWindow(), &m_Size[0], &m_Size[1]);
			glViewport(0, 0, shadowMap->size.x, shadowMap->size.y);
			shadowMap->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);
			Renderer::BakeShadows();
			Instancing::GetInstance()->BakeShadows();
			shadowMap->UnBind();
		}
		Renderer::s_Stats.m_ShadowRenderTime = Timer::s_OutTime;

		glViewport(0, 0, m_Size[0], m_Size[1]);
		glBindFramebuffer(GL_FRAMEBUFFER, Window::GetInstance().GetCamera().m_FrameBufferColor->m_Fbo);
		Renderer::Clear();
		Renderer::Render();
		Window::GetInstance().GetScrollYOffset() = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Renderer::Clear();
		Instancing::GetInstance()->PrepareVertexAtrrib();

		{
			Timer t;
			Renderer::RenderBloomEffect();
		}
		Renderer::s_Stats.m_BloomRenderTime = Timer::s_OutTime;

		Shader* shader = Shader::Get("QuadFullScreen");
		{
			Timer t;
			Renderer::Clear();
			glBindFramebuffer(GL_FRAMEBUFFER, Window::GetInstance().GetCamera().m_FrameBufferColor->m_Fbo);
			shader->Bind();
			glBindTexture(GL_TEXTURE_2D, Window::GetInstance().GetCamera().m_FrameBufferColor->m_Textures[0]);
			Renderer::RenderForPostEffect(MeshManager::GetInstance().GetMesh("plane"), shader);
			glDisable(GL_DEPTH_TEST);
			glDisable(GL_DEPTH_TEST);
			Renderer::RenderCollision();
			Renderer::RenderCollision3D();
		}
		Renderer::s_Stats.m_CollisionRenderTime = Timer::s_OutTime;

		{
			Timer t;
			Renderer::RenderText();
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			shader->UnBind();
		}
		Renderer::s_Stats.m_GuiRenderTime = Timer::s_OutTime;

		ViewPort::GetInstance()->Update();
		if (ViewPort::GetInstance()->m_IsViewportResized) {
			Camera& camera = Window::GetInstance().GetCamera();
			int* size = Window::GetInstance().GetSize();
			camera.m_FrameBufferColor->Resize  (size[0], size[1]);
			camera.m_FrameBufferBlur[0]->Resize(size[0], size[1]);
			camera.m_FrameBufferBlur[1]->Resize(size[0], size[1]);
		}
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(Window::GetInstance().GetWindow());
		glfwPollEvents();
	}
#ifdef _IS_GAME_BUILD
	m_App->OnClose();
#endif
}

EntryPoint::~EntryPoint() {
	World::GetInstance().ShutDown();
	MeshManager::GetInstance().ShutDown();
	Gui::GetInstance().ShutDown();
	EventSystem::GetInstance().Shutdown();
	ThreadPool::GetInstance().Shutdown();
	Instancing::GetInstance()->ShutDown();
	Texture::ShutDown();
	SoundManager::GetInstance().ShutDown();
	delete &Window::GetInstance().GetCamera();
	
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}