#include "EntryPoint.h"
#include "Core/Timer.h"
#include "Render/Line.h"
using namespace Doom;

EntryPoint::EntryPoint(Doom::Application* app) {
	Window::Init("Doom Engine", 800, 600, false);
	MainThread::Init();
	ThreadPool::Init();
	Editor::Instance()->CheckTexturesFolder("src/Images");
	Batch::Init();
	this->app = app;
	EventSystem::Instance()->SendEvent("OnStart", nullptr);
}
void EntryPoint::Run()
{
	/*Texture* texture1 = new Texture("src/Images/coin.png");
	Texture* texture2 = new Texture("src/Images/bomb.png");
	GameObject** gameobj = new GameObject*[200];
	float x = -10;
	float offset = 2;
	float y = -20;
	for (unsigned int i = 0; i < 200; i++)
	{
		gameobj[i] = new GameObject[200];
		for (unsigned int j = 0; j < 200; j++)
		{
			gameobj[i][j].GetComponentManager()->GetComponent<Transform>()->Translate(offset + x,y);
			if(i % 2 == 0 && j % 2 == 0)
				gameobj[i][j].SetTexture(texture1);
			else
				gameobj[i][j].SetTexture(texture2);
			x += offset;
		}
		x = -10;
		y += 2;
	}
	Window::GetCamera().Zoom(5);*/
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

		Renderer::Render();

		app->OnUpdate();
		if (isEditorEnable)
			Editor::Instance()->EditorUpdate();

		app->OnImGuiRender();

		if (ImGui::IsAnyItemActive())
			Editor::Instance()->isItemActive = true;
		else
			Editor::Instance()->isItemActive = false;

		ImGui::EndFrame();
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(Window::GetWindow());
		glfwPollEvents();
	}
	//delete[] gameobj;
}

EntryPoint::~EntryPoint() {
	app->OnClose();
	delete app;
	EventSystem::Instance()->Shutdown();
	ThreadPool::Instance()->shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
}
