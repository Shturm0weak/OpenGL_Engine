#pragma once

#include <ImGui/imgui.h>
#include <ImGui/examples/imgui_impl_glfw.h>
#include <ImGui/examples/imgui_impl_opengl3.h>
#include "../Render/Renderer.h"
#include <mutex>

namespace Doom {

	class DOOM_API Editor {
	private:

		std::mutex mtx_updateNormals;
		float campos[2] = { 0,0 };
		bool isActiveTextureAtlasCreation = false;
		bool isActiveEditor = false;
		bool tool_active = true;
		bool isActiveMeshPicker = false;
		bool isActiveTexturePicker = false;
		bool isActiveShaderMenu = false;
		int selectedShader = -1;
		int selectedAtlas = -1;
		int selectedanimation = 0;
		int selectedchild = -1;
		int selectedMesh = -1;
		int texturePickerId = -1;
		//int selected = 0;
		int selectedcomponent = 0;
		int closedButtonsId = 103212;
		float* color;
		Texture* textureForAtlas = nullptr;
		float spriteSize[2] = {128,128};
		int axes[3] = { 0,0,1 };
		float changeSliderPos[2] = { -25,25 };
		float changeSliderScale[2] = { 0,25 };
		float changeSliderCollisionOffset[2] = { -10,10 };
		char name[128];
		char tag[128];
		char mesh[128];
		char pathToTextureFolder[64];
		float colOffset[2] = { 0,0 };
		static std::vector<Texture*> s_Texture;
		static std::vector<std::string> s_TexturesPath;
		float uvsOffset[2];
		GameObject* m_PreviousGo = nullptr;

		Editor() {}
		Editor(const Editor&) = delete;
		Editor& operator=(const Editor& rhs) { return *this; }

		void CreateTextureAtlas();
		GameObject* copiedGo = nullptr;

		float TextColor[4] = {1,1,1,1};
		float shadowOffset[2] = {0,0};

		void MenuRenderer3D();
		void MenuCubeCollider3D();
		void MenuRenderer2D();
		void MenuRectangleCollider2D(RectangleCollider2D* col);
		void MenuAnimator2D();
		void MenuTransform(Transform* tr);
		void MenuPointLight();
		void MenuSpotLight();
		void MenuDirectionalLight();
		void MenuSphereCollisionComponent();
		void MenuScriptComponent();
		void MenuParticleEmitterComponent();
		void MenuAllComponents();

		template<class T>
		bool MenuRemoveComponent();
		bool MenuRemoveComponent(Component* com);
		bool MenuRemoveScript(ScriptComponent* sc);
		void MeshPicker();
		void TexturePicker();
		void SceneHierarchy();
		void DrawNode(GameObject* go, ImGuiTreeNodeFlags flags);
		void DrawChilds(GameObject* go);
		void ShaderMenu();
		void MenuShadowMap();
		void MenuInstancingStats();
		void MenuStats();

	public:

		bool isBoundingBoxesVisible = false;
		bool drawNormals = false;
		static std::vector<Texture*> s_TextureVecTemp;
		GameObject* go = nullptr;
		bool isItemActive = false;

		static Editor& GetInstance();
		void ShortCuts();
		void MenuBar();
		void EditorUpdateMyGui();
		void EditorUpdate();
		static void CheckTexturesFolderUnique(const std::string path);
		static void CheckTexturesFolder(const std::string path);
		friend class Renderer;
		friend class Renderer3D;
		void Threads();
		void Debug();
		void TextProps();
		void UpdateNormals(); //@Deprecated
	};

}