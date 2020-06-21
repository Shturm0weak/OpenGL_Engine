#include "EntryPoint.h"
#include "Core/Timer.h"
#include "Render/Line.h"
#include "Audio/SoundManager.h"
#include "Core/Editor.h"

using namespace Doom;

EntryPoint::EntryPoint(Doom::Application* app) {
	if (app == nullptr)
		this->app = new Application();
	else
		this->app = app;
	Window::Init(app->name.c_str(),app->width,app->height,app->Vsync);
	MainThread::Init();
	ThreadPool::Init();
	Batch::Init();
	SoundManager::Init();
	Gui::GetInstance()->LoadStandartFonts();
	EventSystem::GetInstance()->SendEvent("OnStart", nullptr);
	Window::GetCamera().frameBuffer = new FrameBuffer();
	Shader::defaultShader = new Shader("src/Shaders/Basic.shader");
}

void EntryPoint::Run()
{
	bool isEditorEnable = false;
	app->OnStart();

	bool FirstFrame = true;

	while (!glfwWindowShouldClose(Window::GetWindow())) {
		DeltaTime::calculateDeltaTime();

		if (FirstFrame) {
			DeltaTime::deltatime = 0.000001;
			FirstFrame = false;
		}
 		Renderer::DrawCalls = 0;
		Renderer::Vertices = 0;
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		Window::GetCamera().WindowResize();
		Window::GetCamera().CameraMovement();
		SoundManager::UpdateSourceState();

		if (Input::IsKeyPressed(Keycode::KEY_E)) {
			if (isEditorEnable)
				isEditorEnable = false;
			else
				isEditorEnable = true;
		}
		
		if(Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
			Renderer::SelectObject();

		app->OnUpdate();

		Gui::GetInstance()->Begin();
		app->OnGuiRender();
		Gui::GetInstance()->End();
		
		if (ImGui::IsAnyItemActive())
			Editor::Instance()->isItemActive = true;
		else
			Editor::Instance()->isItemActive = false;

		glBindFramebuffer(GL_FRAMEBUFFER, Window::GetCamera().frameBuffer->fbo);
		Renderer::Clear();
		Renderer::Render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Renderer::Clear();

		ViewPort::GetInstance()->Update();

		if (isEditorEnable)
			Editor::Instance()->EditorUpdate();
		
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ViewPort::GetInstance()->viewportResized) {
			Window::GetCamera().frameBuffer->Resize(ViewPort::GetInstance()->GetSize().x, ViewPort::GetInstance()->GetSize().y);
		}
		EventSystem::GetInstance()->ProcessEvents();
		Input::Clear();
		glfwSwapBuffers(Window::GetWindow());
		glfwPollEvents();
	}
	app->OnClose();
	MeshManager::ShutDown();
	Renderer::ShutDown();
	delete app;
	
}

EntryPoint::~EntryPoint() {

	EventSystem::GetInstance()->Shutdown();
	ThreadPool::Instance()->Shutdown();
	Texture::ShutDown();
	SoundManager::ShutDown();
	
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}