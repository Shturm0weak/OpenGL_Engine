#include "EntryPoint.h"
#include "Core/Timer.h"
#include "Render/Line.h"
using namespace Doom;

EntryPoint::EntryPoint(Doom::Application* app) {
	Window::Init("Doom Engine", 800, 600, false);
	MainThread::Init();
	ThreadPool::Init();
	Batch::Init();
	this->app = app;
	EventSystem::Instance()->SendEvent("OnStart", nullptr);
}
void EntryPoint::Run()
{
	bool isEditorEnable = false;
	double editortimer = 1;
	app->OnStart();
	while (!glfwWindowShouldClose(Window::GetWindow())) {
		
		Renderer::DrawCalls = 0;
		//Renderer::CalculateMVPforAllObjects();
		Renderer::Clear();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		DeltaTime::calculateDeltaTime();
		Window::GetCamera().WindowResize();

		EventSystem::Instance()->ProcessEvents();
		Window::GetCamera().CameraMovement();
		if (editortimer > 0.2 && Input::IsKeyPressed(Keycode::KEY_E)) {
			if (isEditorEnable)
				isEditorEnable = false;
			else
				isEditorEnable = true;
			editortimer = 0;
		}
		editortimer += DeltaTime::deltatime;

		app->OnUpdate();
		if (isEditorEnable)
			Editor::Instance()->EditorUpdate();

		app->OnImGuiRender();

		if (ImGui::IsAnyItemActive())
			Editor::Instance()->isItemActive = true;
		else
			Editor::Instance()->isItemActive = false;

		Renderer::Render();

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(Window::GetWindow());
		glfwPollEvents();
	}
	app->OnClose();
}

EntryPoint::~EntryPoint() {
	
	delete app;
	EventSystem::Instance()->Shutdown();
	ThreadPool::Instance()->shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
