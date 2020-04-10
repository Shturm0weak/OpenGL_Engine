#include "EntryPoint.h"
#include "Core/Timer.h"
#include "Render/Line.h"

using namespace Doom;

EntryPoint::EntryPoint(Doom::Application* app) {
	MainThread::Init();
	Window::Init(800, 600);
	ThreadPool::Init();
	Editor::Instance()->CheckTexturesFolder("C:/Users/Alexandr/source/repos/DoomEngine/SandBox/src/Images");
	Batch::Init();
	this->app = app;
	EventSystem::Instance()->SendEvent("OnStart", nullptr);
}

void EntryPoint::Run()
{
	/*Texture* texture1 = new Texture("src/Images/coin.png");
	Texture* texture2 = new Texture("src/Images/bomb.png");
	GameObject** gameobj = new GameObject*[10];
	float x = -10;
	float offset = 2;
	float y = -20;
	for (unsigned int i = 0; i < 10; i++)
	{
		gameobj[i] = new GameObject[10];
		for (unsigned int j = 0; j < 10; j++)
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
		Renderer::CalculateMVPforAllObjects();
		Renderer::Clear();
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
		Renderer::SubmitGameObjects(Window::GetCamera());
		Renderer::RenderCollision(Window::GetCamera());
		Renderer::RenderText();
		if (isEditorEnable)
			Editor::Instance()->EditorUpdate();

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
