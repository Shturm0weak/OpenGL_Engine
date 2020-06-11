#include "EntryPoint.h"
#include "Core/Timer.h"
#include "Render/Line.h"
#include "Audio/SoundManager.h"
#include "Core/Editor.h"

using namespace Doom;

EntryPoint::EntryPoint(Doom::Application* app) {
	Window::Init("Doom Engine", 640, 360, false);
	MainThread::Init();
	ThreadPool::Init();
	Batch::Init();
	SoundManager::Init();
	Gui::GetInstance()->LoadStandartFonts();
	EventSystem::Instance()->SendEvent("OnStart", nullptr);
	Window::GetCamera().frameBuffer = new FrameBuffer();
	Shader::defaultShader = new Shader("src/Shaders/Basic.shader");
	if (app == nullptr)
		this->app = new Application();
	else
		this->app = app;
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

		app->OnUpdate();
		app->OnImGuiRender();
		
		if (ImGui::IsAnyItemActive())
			Editor::Instance()->isItemActive = true;
		else
			Editor::Instance()->isItemActive = false;

		glBindFramebuffer(GL_FRAMEBUFFER, Window::GetCamera().frameBuffer->fbo);
		Renderer::Clear();
		Renderer::Render();
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Renderer::Clear();

		void* tex = reinterpret_cast<void*>(Window::GetCamera().frameBuffer->texture);

		ImGui::Begin("ViewPort", &ViewPort::Instance()->toolOpen, ImGuiWindowFlags_NoScrollbar);
		if (ImGui::IsWindowFocused())
			ViewPort::Instance()->IsActive = true;
		else
			ViewPort::Instance()->IsActive = false;

		if (ImGui::IsWindowHovered())
			ViewPort::Instance()->IsHovered = true;
		else
			ViewPort::Instance()->IsHovered = false;

		ViewPort::Instance()->SetViewPortPos(ImGui::GetWindowPos().x,ImGui::GetWindowPos().y);
		if (ViewPort::Instance()->GetSize().x != ImGui::GetWindowSize().x || ViewPort::Instance()->GetSize().y != ImGui::GetWindowSize().y) {
			ViewPort::Instance()->SetSize(ImGui::GetWindowSize().x, ImGui::GetWindowSize().y);
			ViewPort::Instance()->viewportResized = true;
		}
		else
			ViewPort::Instance()->viewportResized = false;

		ImGui::GetWindowDrawList()->AddImage(tex, ImVec2(ViewPort::Instance()->GetViewPortPos().x,
        ViewPort::Instance()->GetViewPortPos().y), ImVec2(ViewPort::Instance()->GetViewPortPos().x + ViewPort::Instance()->GetSize().x, ViewPort::Instance()->GetViewPortPos().y + ViewPort::Instance()->GetSize().y), ImVec2(0, 1), ImVec2(1, 0));
		ImGui::End();

		if (isEditorEnable)
			Editor::Instance()->EditorUpdate();
		
		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (ViewPort::Instance()->viewportResized) {
			glBindTexture(GL_TEXTURE_2D, Window::GetCamera().frameBuffer->texture);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, ViewPort::Instance()->GetSize().x, ViewPort::Instance()->GetSize().y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		EventSystem::Instance()->ProcessEvents();
		Input::Clear();
		glfwSwapBuffers(Window::GetWindow());
		glfwPollEvents();
	}
	app->OnClose();
	Renderer::RenderShutDown();
	delete app;
	
}

EntryPoint::~EntryPoint() {

	EventSystem::Instance()->Shutdown();
	ThreadPool::Instance()->shutdown();
	Texture::ShutDown();
	SoundManager::ShutDown();
	
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}