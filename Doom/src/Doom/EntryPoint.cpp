#include "EntryPoint.h"
#include "Core/Timer.h"
#include "Render/Line.h"
#include "Audio/SoundManager.h"
#include "Core/Editor.h"
#include "Objects2D/GridLayOut.h"
#include "Render/SkyBox.h"

using namespace Doom;

EntryPoint::EntryPoint(Doom::Application* app) {
	if (app == nullptr)
		this->app = new Application();
	else
		this->app = app;
	Window::Init(app->name.c_str(),app->width,app->height,app->Vsync);
	app->Init();
	MainThread::Init();
	ThreadPool::Init();
	Texture::WhiteTexture = new Texture();
	Batch::Init();
	SoundManager::Init();
	Font::shader = new Shader("src/Shaders/Text.shader");
	Gui::GetInstance()->LoadStandartFonts();
	EventSystem::GetInstance()->SendEvent("OnStart", nullptr);
	Window::GetCamera().frameBuffer = new FrameBuffer();
	Shader::defaultShader = new Shader("src/Shaders/Basic.shader");
	GridLayOut* grid = new GridLayOut(app);
}

void EntryPoint::Run()
{
	bool isEditorEnable = false;
	bool FirstFrame = true;
	app->OnStart();

	while (!glfwWindowShouldClose(Window::GetWindow())) {
		EventSystem::GetInstance()->SendEvent("OnUpdate", nullptr);

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
			isEditorEnable = !isEditorEnable;
		}
		
		if (app->type == TYPE_2D) {
			if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
				Renderer::SelectObject();
		}

		app->OnUpdate();

		Gui::GetInstance()->Begin();
		app->OnGuiRender();
		Gui::GetInstance()->End();

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
	Gui::GetInstance()->ShutDown();
	EventSystem::GetInstance()->Shutdown();
	ThreadPool::Instance()->Shutdown();
	Texture::ShutDown();
	SoundManager::ShutDown();
	
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}