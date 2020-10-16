#include "EntryPoint.h"
#include "Core/Timer.h"
#include "Render/Line.h"
#include "Core/World.h"
#include "Audio/SoundManager.h"
#include "Core/Editor.h"
#include "Objects2D/GridLayOut.h"
#include "Render/SkyBox.h"
#include "Render/MeshManager.h"
#include "Render/Instancing.h"

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
	Shader::Create("BakeShadows", "src/Shaders/BakeShadows.shader"); 
	Shader::Create("Instancing3DBakeShadows", "src/Shaders/BakeShadowsInstancing.shader");
	Batch::Init();
	SoundManager::Init();
	Font::shader = Shader::Get("Font");
	Gui::GetInstance()->LoadStandartFonts();
	EventSystem::GetInstance()->SendEvent("OnStart", nullptr);
	Window::GetCamera().frameBufferColor = new FrameBuffer(Window::GetSize()[0], Window::GetSize()[1], GL_RGB,GL_UNSIGNED_BYTE,false, GL_COLOR_ATTACHMENT0,true,true,true);
	Window::GetCamera().frameBufferShadowMap = new FrameBuffer(4096, 4096, GL_DEPTH_COMPONENT, GL_FLOAT, true, GL_DEPTH_ATTACHMENT, false, false, false);
	if (app->type == TYPE_3D) {
		GridLayOut* grid = new GridLayOut();
		Editor::Instance()->gizmo = new Gizmos;
	}
	
}

void EntryPoint::Run()
{
	bool isEditorEnable = false;
	bool FirstFrame = true;
	app->OnStart();

	while (!glfwWindowShouldClose(Window::GetWindow())) {
		Gui::GetInstance()->isAnyPanelHovered = false;
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

		EventSystem::GetInstance()->ProcessEvents();

		if (Input::IsKeyPressed(Keycode::KEY_E)) {
			isEditorEnable = !isEditorEnable;
		}
		
		if (app->type == TYPE_2D) {
			if (Input::IsMousePressed(Keycode::MOUSE_BUTTON_1))
				Renderer::SelectObject();
		}

		Renderer::UpdateLightSpaceMatrices();
		app->OnUpdate();
		Gui::GetInstance()->Begin();
		app->OnGuiRender();
		Gui::GetInstance()->End();

		if (isEditorEnable) {
			Editor::Instance()->EditorUpdate();
		}
		else {
			Gui::GetInstance()->Begin();
			Editor::Instance()->EditorUpdateMyGui();
			Gui::GetInstance()->End();
		}
		
		Input::Clear();

		Renderer::DrawCalls = 0;
		Renderer::Vertices = 0;

		if (Instancing::Instance()->drawShadows) {
			FrameBuffer* shadowMap = Window::GetCamera().frameBufferShadowMap;
			glfwGetWindowSize(Window::GetWindow(), &size[0], &size[1]);
			glViewport(0, 0, shadowMap->size.x, shadowMap->size.y);
			shadowMap->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);
			Renderer::BakeShadows();
			Instancing::Instance()->BakeShadows();
			shadowMap->UnBind();
		}

		//void* my_tex_id = reinterpret_cast<void*>(shadowMap->texture);
		//ImGui::Begin("FrameBuffer");
		//ImGui::Image(my_tex_id,ImVec2(512,512),ImVec2(0,1),ImVec2(1,0));
		//ImGui::SliderFloat("Znear", &Window::GetCamera().znears, -500, 500);
		//ImGui::SliderFloat("Zfar", &Window::GetCamera().zfars, 0, 500);
		//ImGui::SliderFloat("Projection", &Window::GetCamera().rationprojections, 0, 1000);
		//ImGui::SliderFloat("DrawShadows", &Instancing::Instance()->drawShadows, 0, 1);
		//ImGui::End();

		glViewport(0, 0, size[0], size[1]);
		glBindFramebuffer(GL_FRAMEBUFFER, Window::GetCamera().frameBufferColor->fbo);
		Renderer::Clear();
		Renderer::Render();
		Window::GetScrollYOffset() = 0;
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		Renderer::Clear();
		Instancing::Instance()->PrepareVertexAtrrib();

		ViewPort::GetInstance()->Update();

		if (ViewPort::GetInstance()->viewportResized) {
			Window::GetCamera().frameBufferColor->Resize(ViewPort::GetInstance()->GetSize().x, ViewPort::GetInstance()->GetSize().y);
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