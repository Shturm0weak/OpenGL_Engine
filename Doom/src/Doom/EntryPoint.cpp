#include "EntryPoint.h"
#include "Core/Timer.h"
#include "Render/Line.h"
#include "Audio/SoundManager.h"
#include "Core/Editor.h"
#include "Objects2D/GridLayOut.h"
#include "Render/SkyBox.h"
#include "Render/MeshManager.h"

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
	Texture::WhiteTexture = Texture::ColoredTexture("WhiteTexture",0xFFFFFFFF);
	Texture::ColoredTexture("InvalidTexture", 0x8D329Fff);
	Shader::Create("Font", "src/Shaders/Text.shader");
	Shader::Create("Default2D", "src/Shaders/Basic.shader");
	Shader::Create("Default3D", "src/Shaders/Basic3D.shader");
	Shader::Create("Instancing3D", "src/Shaders/Instancing3D.shader");
	Batch::Init();
	SoundManager::Init();
	Font::shader = Shader::Get("Font");
	Gui::GetInstance()->LoadStandartFonts();
	EventSystem::GetInstance()->SendEvent("OnStart", nullptr);
	Window::GetCamera().frameBuffer = new FrameBuffer();
	GridLayOut* grid = new GridLayOut(app);
}

void EntryPoint::Run()
{
	bool isEditorEnable = false;
	bool FirstFrame = true;
	app->OnStart();
	while (!glfwWindowShouldClose(Window::GetWindow())) {
		EventSystem::GetInstance()->SendEvent("OnUpdate", nullptr);
		ThreadPool::Instance()->enqueue([] {Editor::Instance()->UpdateNormals(); });
		DeltaTime::calculateDeltaTime();

		if (FirstFrame) {
			DeltaTime::deltatime = 0.000001;
			FirstFrame = false;
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
		ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());

		Window::GetCamera().WindowResize();
		Window::GetCamera().CameraMovement();
		MeshManager::DispatchLoadedMeshes();
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

		if (isEditorEnable)
			Editor::Instance()->EditorUpdate();

		EventSystem::GetInstance()->ProcessEvents();
		Input::Clear();

		Renderer::DrawCalls = 0;
		Renderer::Vertices = 0;

		glBindFramebuffer(GL_FRAMEBUFFER, Window::GetCamera().frameBuffer->fbo);
		Renderer::Clear();
		Renderer::Render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Renderer::Clear();

		ViewPort::GetInstance()->Update();

		if (ViewPort::GetInstance()->viewportResized) {
			Window::GetCamera().frameBuffer->Resize(ViewPort::GetInstance()->GetSize().x, ViewPort::GetInstance()->GetSize().y);
		}

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

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