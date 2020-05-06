#ifndef EDITOR_H
#define EDITOR_H
#include <ImGui/imgui.h>
#include <ImGui/examples/imgui_impl_glfw.h>
#include <ImGui/examples/imgui_impl_opengl3.h>
#include "../Render/Renderer.h"
#include <mutex>

namespace Doom {

	class DOOM_API Editor {
	private:
		float campos[2] = { 0,0 };
		bool IsActiveEditor = false;
		bool tool_active = true;
		int selectedanimation = 0;
		int selectedchild = -1;
		int selected = 0;
		int selectedcomponent = 0;
		float* color;
		int axes[3] = { 0,0,1 };
		float changeSliderPos[2] = { -10,10 };
		float changeSliderScale[2] = { 0,10 };
		float changeSliderCollisionOffset[2] = { -10,10 };
		char name[128];
		char tag[128];
		float colOffset[2] = { 0,0 };
		static std::vector<Texture*> texture;
		static std::vector<std::string> texturesPath;
		float uvsOffset[2];
		Editor& operator=(const Editor& rhs) { return *this; }
		Editor() {}
	public:
		bool isItemActive = false;
		glm::vec4 ColorPickUp(float* c);
		static Editor* Instance();
		void EditorUpdate();
		static void CheckTexturesFolder(const std::string path);
		friend class Doom::Renderer;
	};

}
#endif