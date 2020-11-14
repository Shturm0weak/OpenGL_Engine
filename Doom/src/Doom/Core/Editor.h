#ifndef EDITOR_H
#define EDITOR_H
#include <ImGui/imgui.h>
#include <ImGui/examples/imgui_impl_glfw.h>
#include <ImGui/examples/imgui_impl_opengl3.h>
#include "../Render/Renderer.h"
#include <mutex>
#include "../ImGuizmo/Gizmos.h"

namespace Doom {

	class DOOM_API Editor {
	private:
		std::mutex mtx_updateNormals;
		float campos[2] = { 0,0 };
		bool isBoundingBoxesVisible = false;
		bool IsActiveTextureAtlasCreation = false;
		bool IsActiveEditor = false;
		bool tool_active = true;
		bool IsActiveMeshPicker = false;
		bool IsActiveTexturePicker = false;
		bool IsActiveShaderMenu = false;
		int previousSelectedGo = -1;
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
		static std::vector<Texture*> texture;
		static std::vector<std::string> texturesPath;
		float uvsOffset[2];
		Editor& operator=(const Editor& rhs) { return *this; }
		Editor() {}
		void CreateTextureAtlas();

		float TextColor[4] = {255,255,255,255};
		float shadowOffset[2] = {0,0};
		std::vector<Line*> normals;

		void MenuRenderer3D();
		void MenuCubeCollider3D();
		void MenuRenderer2D();
		void MenuRectangleCollider2D(RectangleCollider2D* col);
		void MenuAnimator2D();
		void MenuTransform(Transform* tr);
		void MenuLightPoint();
		void MenuDirectionalLight();
		void MenuSphereCollisionComponent();
		void MenuScriptComponent();

		template<class T>
		bool MenuRemoveComponent();

		bool MenuRemoveScript(ScriptComponent* sc);
		void MeshPicker();
		void TexturePicker();
		void SceneHierarchy();
		void DrawNode(GameObject* go, ImGuiTreeNodeFlags flags);
		void DrawChilds(GameObject* go);
		void ShaderMenu();

	public:
		bool drawNormals = false;
		Gizmos* gizmo = nullptr;
		static std::vector<Texture*> textureVecTemp;
		GameObject* go = nullptr;
		bool isItemActive = false;
		static Editor* Instance();
		void MenuBar();
		void EditorUpdateMyGui();
		void EditorUpdate();
		static void CheckTexturesFolderUnique(const std::string path);
		static void CheckTexturesFolder(const std::string path);
		friend class Doom::Renderer;
		friend class Renderer3D;
		void Threads();
		void Debug();
		void TextProps();
		void UpdateNormals();
	};
}
#endif