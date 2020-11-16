#include "../pch.h"
#include "Editor.h"
#include <iostream>
#include <filesystem>
#include "../Enums/ColoredOutput.h"
#include "../Text/Gui.h"
#include "../Core/World.h"
#include "../Render/Instancing.h"
#include "Components/ScriptComponent.h"
#include "Core/Utils.h"

namespace fs = std::filesystem;

using namespace Doom;

template<class T>
bool Editor::MenuRemoveComponent()
{
	ImGui::PushID(closedButtonsId);
	if (ImGui::Button("x")) {
		go->GetComponentManager()->RemoveComponent<T>();
		ImGui::PopID();
		return false;
	}
	ImGui::PopID();
	closedButtonsId++;
	ImGui::SameLine();
	return true;
}

void Editor::EditorUpdate()
{
	
	closedButtonsId = 103212;
	Debug();
	Threads();

	ImGui::Begin("Console");

	ImGui::SameLine();
	if (ImGui::Button("EXIT")) {
		glfwSetWindowShouldClose(Window::GetWindow(), GLFW_TRUE);
	}
	if (ImGui::Button("Create Atlas")) {
		isActiveTextureAtlasCreation = !isActiveTextureAtlasCreation;
	}

	if (ImGui::Button("Shaders")) {
		isActiveShaderMenu = true;
	}

	ImGui::SliderFloat("DrawShadows", &Instancing::Instance()->drawShadows, 0, 1);

	CreateTextureAtlas();

	ImGui::SliderFloat("Zoom", &Window::GetCamera().zoomlevel, 0.1f, 100.f);
	Window::GetCamera().Zoom(abs(Window::GetCamera().GetZoomLevel()));
	ImGui::End();

	ImGui::Begin("Scene");
	Editor::GetInstance()->MenuBar();
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("Create GameObject"))
		{
			Renderer::CreateGameObject();
			Renderer::CalculateObjectsVectors().size();
			go = World::objects.back();
		}
		if (ImGui::MenuItem("Create 2D GameObject"))
		{
			Renderer::CreateGameObject();
			Renderer::CalculateObjectsVectors().size();
			go = World::objects.back();
			go->GetComponentManager()->AddComponent<SpriteRenderer>();
		}
		if (ImGui::MenuItem("Create 3D GameObject"))
		{
			Renderer::CreateGameObject();
			Renderer::CalculateObjectsVectors().size();
			go = World::objects.back();
			go->GetComponentManager()->AddComponent<Renderer3D>()->LoadMesh(MeshManager::GetMesh("cube"));
		}
		if (ImGui::MenuItem("Clone"))
		{
			GameObject* clgo = Renderer::CreateGameObject();
			clgo->operator=(*go);
			go = clgo;
		}
			
		if (ImGui::MenuItem("Delete"))
		{
			if (go != nullptr) {
				Renderer::DeleteObject(go->id);
				if(World::objects.size() > 0)
					gizmo->obj = World::objects[World::objects.size() - 1];
				else
					gizmo->obj = nullptr;
				go = gizmo->obj;
			}
			ImGui::EndPopup();
			ImGui::End();
			return;
		}
		ImGui::EndPopup();
	}

	ImGui::SetWindowFontScale(1.25);
	Renderer::CalculateObjectsVectors();

	if (gizmo != nullptr && gizmo->obj != nullptr) {
		go = gizmo->obj;
	}
	else if (gizmo != nullptr && go != nullptr) {
		gizmo->obj = go;
	}

	if (World::objects.size() > 0 && go == nullptr) {
		go = World::objects[0];
	}

	SceneHierarchy();

	ImGui::NewLine();
	ImGui::End();
	ImGui::Begin("Properties");
	 {
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Renderer3D"))
			{
				go->GetComponentManager()->AddComponent<Renderer3D>();
			}
			if (ImGui::MenuItem("SpriteRenderer"))
			{
				go->GetComponentManager()->AddComponent<SpriteRenderer>();
			}
			if (ImGui::MenuItem("RectangleCollider2D"))
			{
				go->GetComponentManager()->AddComponent<Doom::RectangleCollider2D>();
			}
			if (ImGui::MenuItem("DirectionalLight"))
			{
				go->GetComponentManager()->AddComponent<DirectionalLight>();
			}
			if (ImGui::MenuItem("PointLight"))
			{
				go->GetComponentManager()->AddComponent<PointLight>();
			}
			if (ImGui::MenuItem("CubeCollider3D"))
			{
				go->GetComponentManager()->AddComponent<CubeCollider3D>();
			}
			if (ImGui::MenuItem("SphereCollider3D"))
			{
				go->GetComponentManager()->AddComponent<SphereCollider>();
			}
			if (ImGui::MenuItem("ScriptComponent")) {
				go->GetComponentManager()->AddComponent<ScriptComponent>();
			}
			if (ImGui::MenuItem("Add child")) {

				GameObject* obj = Renderer::CreateGameObject();
				Renderer::CalculateObjectsVectors().size();
				obj->SetOwner((void*)go);
				go->AddChild((void*)obj);
				go = World::objects.back();
				go->GetComponentManager()->AddComponent<Renderer3D>()->LoadMesh(MeshManager::GetMesh("cube"));
			}
			ImGui::EndPopup();
		}
		ImGui::SetWindowFontScale(1.25);
		if (Renderer::GetObjectsWithNoOwnerReference().size() > 0) {
			if (go == nullptr)
				return;
			Doom::Transform* tr = go->component_manager->GetComponent<Doom::Transform>();
			Doom::RectangleCollider2D* col = go->component_manager->GetComponent<Doom::RectangleCollider2D>();
			ImGui::Text("ID %d", go->id);
			ImGui::Checkbox("Enable", &go->Enable);
			if (go->GetOwner() != nullptr)
				ImGui::Text("Has Owner");
			ImGui::InputText("Name", name, sizeof(name));
			ImGui::SameLine();
			if (ImGui::Button("Change name")) {
				go->name = name;
			}
			ImGui::SliderInt("Layer", &go->GetLayer(), 0, Renderer::GetAmountOfObjects() - 1);
			if (go->GetComponentManager()->GetComponent<SpriteRenderer>() != nullptr && go->GetComponentManager()->GetComponent<SpriteRenderer>()->renderType == TYPE_2D) {
				if (ImGui::Button("Change layer")) {
					if (go->GetLayer() > Renderer::GetAmountOfObjects() - 1) {
						std::cout << "Error: layer out of range" << std::endl;
						return;
					}
					go->GetComponent<SpriteRenderer>()->Setlayer(go->GetLayer());
					Renderer::CalculateObjectsVectors();
				}
			}

			MenuTransform(tr);
			MenuRenderer2D();
			MenuAnimator2D();
			MenuCubeCollider3D();
			MenuRectangleCollider2D(col);
			MenuLightPoint();
			MenuDirectionalLight();
			MenuRenderer3D();
			MeshPicker();
			TexturePicker();
			ShaderMenu();
			MenuScriptComponent();
			MenuSphereCollisionComponent();
		}
	}
	ImGui::End();
}
void Editor::CreateTextureAtlas() {
	if (!isActiveTextureAtlasCreation)
		return;
	ImGui::Begin("Texture Atlas creation", &isActiveTextureAtlasCreation);
	ImGui::InputFloat2("Sprite size", spriteSize);
	ImGui::InputText("Path to texture folder from solution directory", pathToTextureFolder, 64);
	if(ImGui::Button("Check Folder")) {
		std::string path = pathToTextureFolder;
		CheckTexturesFolder(path);
	}

	int counterImagesButtons = 0;
	ImGui::Text("Textures");
	for (auto i = Texture::textures.begin(); i != Texture::textures.end();i++)
	{
		void* my_tex_id = reinterpret_cast<void*>(i->second->m_RendererID);
		int frame_padding = -1;
		if (counterImagesButtons > 6) {
			ImGui::NewLine();
			counterImagesButtons = 0;
		}

		if (ImGui::ImageButton(my_tex_id, ImVec2(36, 36), ImVec2(1, 1), ImVec2(0, 0), frame_padding, ImVec4(1.0f, 1.0f, 1.0f, 0.5f))) {
			textureForAtlas = i->second;
		}

		ImGui::SameLine();
		counterImagesButtons++;
	}

	ImGui::NewLine();
	if (ImGui::Button("Apply") &&  textureForAtlas != nullptr) {
		isActiveTextureAtlasCreation = false;
		std::string name = "TextureAtlas/" + textureForAtlas->GetFilePath();
		TextureAtlas* textureAtlas = TextureAtlas::CreateTextureAtlas(name,spriteSize[0],spriteSize[1],textureForAtlas);
	}
	ImGui::End();
}

void Doom::Editor::MenuRenderer3D()
{
	if (go->GetComponentManager()->GetComponent<Renderer3D>() != nullptr && go->GetComponentManager()->GetComponent<Renderer3D>()->renderType == TYPE_3D) {
		if (MenuRemoveComponent<Renderer3D>()) {
			if (ImGui::CollapsingHeader("Renderer 3D")) {
				Renderer3D* r = go->GetComponentManager()->GetComponent<Renderer3D>();
				ImGui::Indent(ImGui::GetWindowSize().x * 0.05);
				ImGui::Checkbox("Cast shadows", &r->isCastingShadows);
				ImGui::Checkbox("Wire mesh", &r->isWireMesh);
				if (ImGui::CollapsingHeader("Shader")) {
					Shader* shader = go->GetComponentManager()->GetComponent<Renderer3D>()->shader;
					ImGui::Text("%s", shader->m_Name);
					if (ImGui::Button("Reload")) {
						shader->Reload();
					}
					ImGui::InputText("Shader's name", this->name, sizeof(this->name));
					if (ImGui::Button("Change shader")) {
						Shader* shaderTemp = Shader::Get(this->name);
						if (shaderTemp != nullptr) {
							r->shader = shaderTemp;
						}
					}
				}
				if (ImGui::CollapsingHeader("Material")) {
					if (!r->isTransparent) {
						if (ImGui::Button("Make Transparent")) {
							r->MakeTransparent();
						}
					}
					else {
						if (ImGui::Button("Make Solid")) {
							r->MakeSolid();
						}
					}

					ImGui::SliderFloat("Ambient", &r->mat.ambient, 0, 1);
					ImGui::SliderFloat("Specular", &r->mat.specular, 0, 50);
					void* my_tex_id = reinterpret_cast<void*>(r->diffuseTexture->m_RendererID);
					if (ImGui::ImageButton(my_tex_id, { 64,64 }, ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0.79, 0, 0.75, 1))) {
						isActiveTexturePicker = true;
						texturePickerId = 1;
					}
					if (r->normalMapTexture == nullptr)
						my_tex_id = reinterpret_cast<void*>(Texture::Get("InvalidTexture")->m_RendererID);
					else
						my_tex_id = reinterpret_cast<void*>(r->normalMapTexture->m_RendererID);
					if (ImGui::ImageButton(my_tex_id, { 64,64 }, ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0.79, 0, 0.75, 1))) {
						isActiveTexturePicker = true;
						texturePickerId = 2;
					}
					ImGui::Checkbox("NormalMap", &r->useNormalMap);
					float* tempColor = r->GetColor();
					ImGui::ColorEdit4("Color", tempColor);
					r->SetColor(glm::vec4(tempColor[0], tempColor[1], tempColor[2], tempColor[3]));
				}
				if (ImGui::CollapsingHeader("Mesh")) {
					if (r->mesh != nullptr) {
						ImGui::Text("Name: %s", r->mesh->name);
					}
					if (ImGui::Button("Meshes")) {
						isActiveMeshPicker = true;
					}
				}
				ImGui::Unindent();
			}
		}
	}
}

void Doom::Editor::MenuCubeCollider3D()
{
	CubeCollider3D* cc = go->GetComponentManager()->GetComponent<CubeCollider3D>();
	if (cc != nullptr && cc->isBoundingBox == false) {
		if (MenuRemoveComponent<CubeCollider3D>()) {
			if (ImGui::CollapsingHeader("CubeCollider3D")) {
				ImGui::SliderFloat3("Position", &cc->offset.x, -50, 50);
			}
		}
	}
}

void Doom::Editor::MenuRenderer2D()
{
	if (go->GetComponentManager()->GetComponent<SpriteRenderer>() != nullptr && go->GetComponentManager()->GetComponent<SpriteRenderer>()->renderType == TYPE_2D) {
		SpriteRenderer* sr = go->GetComponent<SpriteRenderer>();
		if (MenuRemoveComponent<SpriteRenderer>()) {
			if (ImGui::CollapsingHeader("Render2D")) {
				color = sr->GetColor();
				ImGui::ColorEdit4("Sprite color", color);
				sr->SetColor(glm::vec4(color[0], color[1], color[2], color[3]));
				delete[] color;
				int counterImagesButtons = 0;
				ImGui::Text("Textures");
				for (auto i = Texture::textures.begin(); i != Texture::textures.end(); i++)
				{
					void* my_tex_id = reinterpret_cast<void*>(i->second->m_RendererID);
					int frame_padding = -1;
					if (counterImagesButtons > 6) {
						ImGui::NewLine();
						counterImagesButtons = 0;
					}

					if (ImGui::ImageButton(my_tex_id, ImVec2(36, 36), ImVec2(1, 1), ImVec2(0, 0), frame_padding, ImVec4(1.0f, 1.0f, 1.0f, 0.5f))) {
						if (go != nullptr) {

							sr->SetTexture(i->second);
						}

					}

					ImGui::SameLine();
					counterImagesButtons++;
				}

				ImGui::NewLine();
				ImGui::InputFloat2("UVs Offset", uvsOffset);

				if (ImGui::Button("Use these UVs")) {
					if (go != nullptr && sr->textureAtlas != nullptr)
						sr->SetUVs(sr->textureAtlas->GetSpriteUVs(uvsOffset[0], uvsOffset[1]));
				}
				if (ImGui::Button("Original UVs")) {
					if (go != nullptr)
						sr->OriginalUvs();
				}
				if (ImGui::Button("No texture")) {
					if (go != nullptr)
						sr->SetTexture(Texture::WhiteTexture);
				}
				if (ImGui::Button("Load texture ...")) {
					std::optional<std::string> filePath = FileDialogs::OpenFile("textures (*.png)\0");
					if (filePath) {
						Texture::AsyncLoadTexture(*filePath);
					}
				}
				int prevselectedAtlas = selectedAtlas;
				if (TextureAtlas::textureAtlases.size() > 0) {
					ImGui::ListBox("Texture atlases", &selectedAtlas, TextureAtlas::GetTextureAtlases(), TextureAtlas::textureAtlases.size());
				}
				if (selectedAtlas != -1) {
					if (selectedAtlas != prevselectedAtlas)
						tool_active = true;
					if (tool_active) {
						ImGui::Begin("Texture Atlas", &tool_active);

						Texture* textureOfAtlas = TextureAtlas::GetTextureAtlas(selectedAtlas)->GetTexture();
						int frame_padding = -1;
						unsigned int amountOfSpritesX = (textureOfAtlas->GetWidth()) / (TextureAtlas::GetTextureAtlas(selectedAtlas)->GetSpriteWidth());
						unsigned int amountOfSpritesY = (textureOfAtlas->GetHeight()) / (TextureAtlas::GetTextureAtlas(selectedAtlas)->GetSpriteHeight());
						for (unsigned int i = 0; i < amountOfSpritesY; i++)
						{
							for (unsigned int j = 0; j < amountOfSpritesX; j++)
							{
								float* uvs = TextureAtlas::GetTextureAtlas(selectedAtlas)->GetSpriteUVs(j, amountOfSpritesY - i);
								ImGui::PushID((i * amountOfSpritesX) + j);
								if (ImGui::ImageButton((void*)(intptr_t)textureOfAtlas->m_RendererID, ImVec2(56, 56), ImVec2(uvs[0], uvs[5]), ImVec2(uvs[4], uvs[1]), frame_padding, ImVec4(1.0f, 1.0f, 1.0f, 0.5f)))
								{
									if (go != nullptr) {
										SpriteRenderer* sr = go->GetComponentManager()->GetComponent<SpriteRenderer>();
										sr->textureAtlas = TextureAtlas::GetTextureAtlas(selectedAtlas);
										sr->SetTexture(textureOfAtlas);
										sr->SetUVs(uvs);
									}
								}
								ImGui::PopID();
								ImGui::SameLine();
							}
							ImGui::NewLine();
						}
						ImGui::NewLine();
						ImGui::InputFloat2("Sprite size", spriteSize);
						if (ImGui::Button("Apply")) {
							TextureAtlas::GetTextureAtlas(selectedAtlas)->SetSpriteSize(spriteSize[0], spriteSize[1]);
						}
						ImGui::End();
					}
					else
						selectedAtlas = -1;
				}

			}
		}
	}
}

void Doom::Editor::MenuRectangleCollider2D(Doom::RectangleCollider2D* col)
{
	if (col != nullptr) {
		if (MenuRemoveComponent<RectangleCollider2D>()) {
			if (ImGui::CollapsingHeader("Rectangle collider2D")) {
				ImGui::Text("Collision");
				ImGui::Text("ID %d", col->GetId());
				ImGui::Text("Tag %s", col->GetTag().c_str());
				ImGui::InputText("Tag", tag, sizeof(tag));
				ImGui::SameLine();
				if (ImGui::Button("Change tag")) {
					col->SetTag(tag);
				}
				ImGui::Checkbox("Enable collision", &col->Enable);
				ImGui::Checkbox("Trigger", &col->IsTrigger);
				ImGui::InputFloat2("Set the borders of X and Y offset slider", changeSliderCollisionOffset);
				ImGui::SliderFloat("Offset X", &col->offset.x, changeSliderCollisionOffset[0], changeSliderCollisionOffset[1]);
				ImGui::SliderFloat("Offset Y", &col->offset.y, changeSliderCollisionOffset[0], changeSliderCollisionOffset[1]);
				ImGui::InputFloat2("Offset", &(col->offset.x, col->offset.x));
				col->SetOffset(col->offset.x, col->offset.y);
				if (ImGui::Button("Remove collider")) {
					go->component_manager->RemoveComponent<Doom::RectangleCollider2D>();
					selectedcomponent = 0;
				}
			}
		}
	}
}

void Doom::Editor::MenuAnimator2D()
{
	if (go->GetComponentManager()->GetComponent<Animator>() != nullptr) {
		if (MenuRemoveComponent<Animator>()) {
			if (ImGui::CollapsingHeader("Animator")) {
				Animator* anim = go->GetComponentManager()->GetComponent<Animator>();
				ImGui::Text("Animator");
				ImGui::Text("counter %d", anim->counter);
				ImGui::ListBox("Animations", &selectedanimation, anim->GetAnimations(), anim->GetAmountOfAnimations());

				int count = 0;
				ImGui::SliderFloat("Animation speed slider", &anim->speed, 0, 100);
				if (anim->GetAmountOfAnimations() > 0) {
					auto iter = anim->animations.find(anim->GetAnimations()[selectedanimation]);
					if (iter._Ptr == nullptr)
						return;
					for (unsigned int i = 0; i < iter->second.size(); i++) {
						void* my_tex_id = reinterpret_cast<void*>((*iter).second[i]->m_RendererID);
						int frame_padding = -1;
						ImGui::Image(my_tex_id, ImVec2(42, 64), ImVec2(1, 1), ImVec2(0, 0), ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
						if (count < 5) {
							ImGui::SameLine();
							count++;
						}
						else
							count = 0;
					}
					anim->selectedanim = selectedanimation;
				}
				ImGui::NewLine();
				ImGui::Checkbox("Play animation", &anim->isPlayingAnim);
			}
		}
	}
}

void Doom::Editor::MenuTransform(Transform* tr)
{
	if (ImGui::Button("o")) {
	}
	ImGui::SameLine();
	if (ImGui::CollapsingHeader("Transform")) {
		ImGui::Text("Position");
		ImGui::InputFloat2("Limits", changeSliderPos);
		glm::vec3 position = go->GetPosition();
		ImGui::SliderFloat("Position X", &(position.x), changeSliderPos[0], changeSliderPos[1]);
		ImGui::SliderFloat("Position Y", &(position.y), changeSliderPos[0], changeSliderPos[1]);
		ImGui::SliderFloat("Position Z", &(position.z), changeSliderPos[0], changeSliderPos[1]);
		ImGui::Text("Scale");
		ImGui::InputFloat2("Limits", changeSliderScale);
		glm::vec3 scale = go->GetScale();
		ImGui::SliderFloat("Scale X", &(scale[0]), changeSliderScale[0], changeSliderScale[1]);
		ImGui::SliderFloat("Scale Y", &(scale[1]), changeSliderScale[0], changeSliderScale[1]);
		ImGui::SliderFloat("Scale Z", &(scale[2]), changeSliderScale[0], changeSliderScale[1]);
		ImGui::Text("Rotate");
		ImGui::SliderAngle("Pitch", &tr->rotation.x);
		ImGui::SliderAngle("Yaw", &tr->rotation.y);
		ImGui::SliderAngle("Roll", &tr->rotation.z);
		tr->Scale(scale[0], scale[1], scale[2]);
		tr->Translate(position.x, position.y, position.z);
		tr->RotateOnce(tr->rotation.x, tr->rotation.y, tr->rotation.z, true);
	}
}

void Doom::Editor::MenuLightPoint()
{
	PointLight* pl = go->GetComponentManager()->GetComponent<PointLight>();
	if (pl == nullptr)
		return;
	if (MenuRemoveComponent<PointLight>()) {
		if (ImGui::CollapsingHeader("Point light")) {
			ImGui::SliderFloat("Constant", &pl->constant, 0, 1);
			ImGui::SliderFloat("Linear", &pl->linear, 0, 0.100f);
			ImGui::SliderFloat("Quadratic", &pl->quadratic, 0, 0.100);
			ImGui::ColorPicker3("Point light color", &pl->color[0]);
		}
	}
}

void Doom::Editor::MenuDirectionalLight()
{
	DirectionalLight* pl = go->GetComponentManager()->GetComponent<DirectionalLight>();
	if (pl == nullptr)
		return;
	if (MenuRemoveComponent<DirectionalLight>()) {
		if (ImGui::CollapsingHeader("Directional light")) {
			ImGui::ColorPicker3("Dir light color", &pl->color[0]);
			ImGui::SliderFloat("Intensity", &pl->intensity, 1, 10);
		}
	}
}

void Doom::Editor::MenuSphereCollisionComponent()
{
	if (go->GetComponentManager()->GetComponent<SphereCollider>() != nullptr) {
		if (MenuRemoveComponent<SphereCollider>()) {
			SphereCollider* cs = go->GetComponentManager()->GetComponent<SphereCollider>();
			if (ImGui::CollapsingHeader("Sphere collider")) {
				ImGui::Checkbox("Is in bounding box", &cs->isInBoundingBox);
				ImGui::SliderFloat("Radius", &cs->radius, 0, 50);
				ImGui::SliderFloat2("Offset", &cs->offset[0], -100, 100);
			}
		}
	}
}

bool Doom::Editor::MenuRemoveScript(ScriptComponent* sc)
{
	ImGui::PushID(closedButtonsId);
	if (ImGui::Button("x")) {
		go->GetComponentManager()->RemoveComponent(sc);
		ImGui::PopID();
		return false;
	}
	ImGui::PopID();
	closedButtonsId++;
	ImGui::SameLine();
	return true;
}

#include "SceneSerializer.h"
#include <optional>

void Doom::Editor::MenuBar()
{
	if (ImGui::BeginMenu("File")) {
		if (ImGui::MenuItem("New")) {
			Renderer::ShutDown();
			gizmo->obj = nullptr;
			go = gizmo->obj;
		}
		if (ImGui::MenuItem("Open")) {
			std::optional<std::string> filePath = FileDialogs::OpenFile("Doom Scene (*.yaml)\0*.yaml\0");
			if (filePath) {
				Renderer::ShutDown();
				SceneSerializer::DeSerialize(*filePath);
				gizmo->obj = nullptr;
				go = gizmo->obj;
			}
		}
		if (ImGui::MenuItem("Save as")) {
			std::optional<std::string> filePath = FileDialogs::SaveFile("Doom Scene (*.yaml)\0*.yaml\0");
			if (filePath) {
				SceneSerializer::Serialize(*filePath);
			}
		}
		ImGui::EndMenu();
	}
}

void Doom::Editor::MeshPicker()
{
	if (!isActiveMeshPicker)
		return;
	ImGui::Begin("Meshes",&isActiveMeshPicker);
	ImGui::ListBox("Meshes",&selectedMesh, MeshManager::GetListOfMeshes(), MeshManager::GetAmountOfMeshes());
	if (ImGui::Button("Apply")) {
		auto mesh = MeshManager::Meshes.begin();
		if (selectedMesh > -1) {
			for (int i = 0; i < selectedMesh; i++)
			{
				mesh++;
			}
		}
		go->GetComponent<Renderer3D>()->LoadMesh(mesh->second);
	}
	if (ImGui::Button("Choose ...")) {
		std::optional<std::string> filePath = FileDialogs::OpenFile("files (*.fbx)\0");
		if (filePath) {
			MeshManager::AsyncLoadMesh(Utils::GetNameFromFilePath(*filePath), *filePath);
		}
	}
	ImGui::End();
}

void Doom::Editor::TexturePicker()
{
	if (!isActiveTexturePicker)
		return;
	ImGui::Begin("Textures", &isActiveTexturePicker);
	int width = 0;
	int imageSize = 64;
	for (auto texture : Texture::textures)
	{
		void* my_tex_id = reinterpret_cast<void*>(texture.second->m_RendererID);
		ImVec2 windowWidth = ImGui::GetWindowSize();
		if (ImGui::ImageButton(my_tex_id, ImVec2(64, 64), ImVec2(1, 1), ImVec2(0, 0), 3, ImVec4(1.0f, 1.0f, 1.0f, 0.5f))) {
			switch (texturePickerId)
			{
			case 0:
				go->GetComponent<SpriteRenderer>()->SetTexture(texture.second);
				break;
			case 1:
				go->GetComponent<Renderer3D>()->diffuseTexture = (texture.second);
				break;
			case 2:
				go->GetComponent<Renderer3D>()->normalMapTexture = (texture.second);
				break;
			default:
				break;
			}
			 
		}
		width += imageSize;
		ImGui::SameLine();
		if (width + imageSize * 2.5 > windowWidth.x) {
			width = 0;
			ImGui::NewLine();
		}
		
	}
	ImGui::End();
}

//void Doom::Editor::SceneHierarchy()
//{
//	if (ImGui::CollapsingHeader("Game Objects")) {
//		unsigned int amount = Renderer::GetAmountOfObjects();
//		for (unsigned int i = 0; i < amount; i++)
//		{
//			GameObject* go = Renderer::objects[i];
//			if (go->GetOwner() != nullptr) {
//				continue;
//			}
//			ImGui::PushID(go->GetId());
//			if (this->go == go && ImGui::IsItemVisible()) {
//				ImVec2 sp = ImGui::GetCursorScreenPos();
//				ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(sp.x, sp.y), ImVec2(ImGui::GetWindowWidth() - 20, sp.y + ImGui::GetItemRectSize().y), IM_COL32(80, 80, 80, 100));
//			}
//			if (ImGui::TreeNodeEx(go->name.c_str())) {
//				if (ImGui::IsItemDeactivated() || ImGui::IsItemActivated())
//					this->go = go;
//				unsigned int childsAmount = go->GetChilds().size();
//				if (childsAmount > 0) {
//					for (unsigned int j = 0; j < childsAmount; j++)
//					{
//						GameObject* child = static_cast<GameObject*>(go->GetChilds()[j]);
//						ImGui::Indent();
//						ImGui::PushID(child->GetId());
//						if (this->go == child && ImGui::IsItemVisible()) {
//							ImVec2 sp = ImGui::GetCursorScreenPos();
//							ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(sp.x, sp.y), ImVec2(ImGui::GetWindowWidth() - 20, sp.y + ImGui::GetItemRectSize().y), IM_COL32(80, 80, 80, 100));
//						}
//						if (ImGui::TreeNodeEx(child->name.c_str())) {
//							if (ImGui::IsItemDeactivated() || ImGui::IsItemActivated())
//								this->go = child;
//							unsigned int childsAmount = child->GetChilds().size();
//							if (childsAmount > 0) {
//								for (unsigned int j = 0; j < childsAmount; j++)
//								{
//									GameObject* child1 = static_cast<GameObject*>(child->GetChilds()[j]);
//									ImGui::PushID(child1->GetId());
//									ImGui::Indent();
//									if (this->go == child1 && ImGui::IsItemVisible()) {
//										ImVec2 sp = ImGui::GetCursorScreenPos();
//										ImGui::GetForegroundDrawList()->AddRectFilled(ImVec2(sp.x, sp.y), ImVec2(ImGui::GetWindowWidth() - 20, sp.y + ImGui::GetItemRectSize().y), IM_COL32(80, 80, 80, 100));
//									}
//									if (ImGui::TreeNodeEx(child1->name.c_str())) {
//										if (ImGui::IsItemDeactivated() || ImGui::IsItemActivated())
//											this->go = child1;
//										ImGui::TreePop();
//									}
//									ImGui::PopID();
//									ImGui::Unindent();
//								}
//							}
//							ImGui::TreePop();
//						}
//						ImGui::PopID();
//						ImGui::Unindent();
//					}
//				}
//				ImGui::TreePop();
//			}
//			ImGui::PopID();
//		}
//	}
//}

void Doom::Editor::SceneHierarchy()
{
	if (ImGui::CollapsingHeader("Game Objects")) {
		uint32_t amount = Renderer::GetAmountOfObjects();
		for (uint32_t i = 0; i < amount; i++)
		{
			GameObject* go = World::objects[i];
			if (go->GetOwner() != nullptr) {
				continue;
			}
			ImGuiTreeNodeFlags flags = ((this->go == go)) ? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Selected : 0;
			DrawNode(go, flags);
		}
	}
}

void Doom::Editor::DrawNode(GameObject* go, ImGuiTreeNodeFlags flags)
{
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)go, flags, go->name.c_str());
	if (ImGui::IsItemClicked()) {
		if(this->gizmo != nullptr)
			this->gizmo->obj = go;
	}

	if (opened) {
		ImGui::TreePop();
		DrawChilds(go);
	}
}

void Doom::Editor::DrawChilds(GameObject * go)
{
	for (uint32_t i = 0; i < go->Childs.size(); i++)
	{
		GameObject* child = static_cast<GameObject*>(go->Childs[i]);
		ImGuiTreeNodeFlags flags = ((this->go == child)) ? ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_Selected : 0;
		ImGui::Indent();
		DrawNode(child, flags);
		ImGui::Unindent();
	}
}

void Doom::Editor::ShaderMenu()
{
	if (!isActiveShaderMenu)
		return;
	ImGui::Begin("Shaders", &isActiveShaderMenu);
	ImGui::ListBox("Shaders", &selectedShader, Shader::GetListOfShaders(), Shader::shaders.size());
	if (ImGui::Button("Reload")) {
		auto shader = Shader::shaders.begin();
		if (selectedShader > -1) {
			for (int i = 0; i < selectedShader; i++)
			{
				shader++;
			}
		}
		shader->second->Reload();
	}
	ImGui::End();
}

void Doom::Editor::MenuScriptComponent()
{
	std::vector<ScriptComponent*> scripts = go->GetComponentManager()->GetScripts();
	for (auto script : scripts)
	{
		if (script != nullptr) {
			ImGui::PushID(script);
			if (MenuRemoveScript(script)) {
				ImGui::SameLine();
				if (ImGui::CollapsingHeader("Script")) {
					if (ImGui::Button("Assign script")) {
						script->AssignScript("src/Scripts/GameTest.lua");
					}
				}
				ImGui::PopID();
			}
			else {
				ImGui::PopID();
				return;
			}
		}
	}
}

void Doom::Editor::CheckTexturesFolderUnique(const std::string path)
{
	auto f = std::bind([](std::string path) {
		try
		{
			for (const auto & entry : fs::directory_iterator(path)) {
				std::string pathToTexture = entry.path().string();
				if (pathToTexture.find(".png") <= pathToTexture.length() || pathToTexture.find(".jpeg") <= pathToTexture.length()) {
					texturesPath.push_back(pathToTexture);
					size_t index = 0;
					index = texturesPath.back().find("\\", index);
					texturesPath.back().replace(index, 1, "/");
					Texture::LoadTextureInRAM(texturesPath.back(), true);
					texture.push_back(Texture::Get(texturesPath.back()));
				}
			}
		}
		catch (const std::exception&)
		{
				
		}
		
		for (unsigned int i = 0; i < texture.size(); i++)
		{
			std::function<void()> f2 = [=] {Texture::LoadTextureInVRAM(texture[i]->GetFilePath()); };
			std::function<void()>* f1 = new std::function<void()>(f2);
			EventSystem::GetInstance()->SendEvent(EventType::ONMAINTHREADPROCESS,nullptr,f1);
		}
		
		
	}, path);
	ThreadPool::GetInstance()->Enqueue(f);
}

void Doom::Editor::CheckTexturesFolder(const std::string path)
{
	auto f = std::bind([](std::string path) {
		try
		{
			for (const auto & entry : fs::directory_iterator(path)) {
				std::string pathToTexture = entry.path().string();
				if (pathToTexture.find(".png") <= pathToTexture.length() || pathToTexture.find(".jpeg") <= pathToTexture.length()) {
					size_t index = 0;
					index = pathToTexture.find("\\", index);
					pathToTexture.replace(index, 1, "/");
					Texture::LoadTextureInRAM(pathToTexture, true);
					Texture* text = Texture::Get(pathToTexture);
					textureVecTemp.push_back(text);
				}
			}
		}
		catch (const std::exception&)
		{

		}

		for (unsigned int i = 0; i < textureVecTemp.size(); i++)
		{
			std::function<void()> f2 = [=] {Texture::LoadTextureInVRAM(textureVecTemp[i]->GetFilePath()); };
			std::function<void()>* f1 = new std::function<void()>(f2);
			EventSystem::GetInstance()->SendEvent(EventType::ONMAINTHREADPROCESS, nullptr, f1);
		}
		textureVecTemp.clear();

	}, path);
	ThreadPool::GetInstance()->Enqueue(f);
}

void Doom::Editor::Debug()
{
	ImGui::Begin("Camera");
	ImGui::SliderAngle("X", &Window::GetCamera().pitch);
	ImGui::SliderAngle("Y", &Window::GetCamera().yaw);
	ImGui::SliderAngle("Z", &Window::GetCamera().roll);
	if (Window::GetCamera().type == Window::GetCamera().PERSPECTIVE) {
		ImGui::SliderAngle("fov", &Window::GetCamera().fovy, 60, 180);
		Window::GetCamera().SetFov(Window::GetCamera().fovy);
	}
	ImGui::Text("Position");
	ImGui::Text("x: %f y: %f z: %f", Window::GetCamera().GetPosition().x, Window::GetCamera().GetPosition().y, Window::GetCamera().GetPosition().z);
	ImGui::Text("Forward vector");
	ImGui::Text("x: %f y: %f z: %f", Window::GetCamera().forwardV.x, Window::GetCamera().forwardV.y, Window::GetCamera().forwardV.z);
	Window::GetCamera().SetRotation(glm::vec3(Window::GetCamera().pitch, Window::GetCamera().yaw, Window::GetCamera().roll));
	ImGui::End();
	ImGui::Begin("Debug");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Objects amount %d", World::objects.size());
	ImGui::Text("Draw calls %d", Renderer::DrawCalls);
	ImGui::Text("Vertices %d", Renderer::Vertices);
	ImGui::Text("VRAM used %lf",Texture::VRAMused);
	ImGui::Text("Textures binded %d", Texture::bindedAmount);
	ImGui::Text("Textures amount %d", Texture::textures.size());
	ImGui::Checkbox("Polygon mode",&Renderer::PolygonMode);
	ImGui::Checkbox("Visible collisions", &RectangleCollider2D::IsVisible);
	ImGui::Checkbox("Visible bounding boxes", &isBoundingBoxesVisible);
	if(gizmo != nullptr)
		ImGui::Checkbox("Round transformations", &Editor::GetInstance()->gizmo->roundTransform);
	ImGui::End();
	TextProps();
}

void Doom::Editor::Threads() {
	ImGui::Begin("Threads");
	for (auto i = ThreadPool::GetInstance()->isThreadBusy.begin(); i != ThreadPool::GetInstance()->isThreadBusy.end(); i++)
	{
		ImGui::Text("ID: %d task: %d",i->first,i->second);
	}
	ImGui::End();
}

void Doom::Editor::TextProps()
{
	Gui* g = Gui::GetInstance();
	ImGui::Begin("Text properties");
	ImGui::SliderFloat("Width", &g->textProps.width,0,0.9);
	ImGui::SliderFloat("Edge", &g->textProps.edge, 0.01, 0.9);
	ImGui::SliderFloat("Border width", &g->textProps.borderwidth, 0, 0.9);
	ImGui::SliderFloat("Border edge", &g->textProps.borderedge, 0.01, 0.9);
	ImGui::ColorPicker4("Out line color", TextColor);
	g->textProps.outLineColor = glm::vec4(TextColor[0], TextColor[1], TextColor[2], TextColor[3]);
	ImGui::SliderFloat2("Shaddow offset", shadowOffset,-0.01,0.01);
	g->textProps.shadowOffset = glm::vec2(shadowOffset[0], shadowOffset[1]);
	ImGui::End();
}

void Doom::Editor::UpdateNormals()
{
	std::lock_guard<std::mutex> lock(mtx_updateNormals);
	if (drawNormals) {
		uint32_t counter = 0;
		size_t size = World::objects.size();
		for (size_t i = 0; i < size; i++)
		{
			Irenderer* iR = World::objects[i]->GetComponent<Irenderer>();
			if (iR->renderType == TYPE_3D) {
				Renderer3D* r = static_cast<Renderer3D*>(iR);
				Mesh* mesh = r->mesh;
				size_t meshSize = r->mesh->meshSize;
				for (size_t j = 0; j < meshSize; j += 14)
				{
					glm::vec3 pos = glm::vec3(mesh->mesh[j + 0], mesh->mesh[j + 1], mesh->mesh[j + 2]);
					glm::vec3 normals = glm::vec3(mesh->mesh[j + 3], mesh->mesh[j + 4], mesh->mesh[j + 5]);
					glm::mat4 scaleXview = r->GetOwnerOfComponent()->GetComponent<Transform>()->scale * r->GetOwnerOfComponent()->GetComponent<Transform>()->view;
					glm::vec4 transformedPos = scaleXview * glm::vec4(pos.x, pos.y, pos.z, 0);
					glm::vec4 transformedNor = scaleXview * glm::vec4(normals.x, normals.y, normals.z, 0);
					this->normals[counter]->SetStartPoint(World::objects[i]->GetPosition() + (glm::vec3)transformedPos);
					this->normals[counter]->SetEndPoint(World::objects[i]->GetPosition() + (glm::vec3)transformedPos + glm::vec3(transformedNor));
					this->normals[counter]->Enable = true;
					counter++;
				}
			}
		}
	}
}

Editor *  Doom::Editor::GetInstance()
{
	static Editor instance;
	return &instance;
}

void Doom::Editor::EditorUpdateMyGui()
{
	Gui* g = Gui::GetInstance();
	DirectionalLight* dirlight = DirectionalLight::dirLights[0];
	GameObject* go = dirlight->GetOwnerOfComponent();
	Transform* tr = go->GetComponent<Transform>();
	if (go != nullptr) {
		g->xAlign = Gui::AlignHorizontally::XCENTER;
		g->Text("FPS: %f", true, 900, 500, 40, COLORS::White, 0, Window::GetFPS());

		g->FontBind(Gui::GetInstance()->GetStandartFonts()[Gui::GetInstance()->ARIAL]);
		g->RelateToPanel();
		g->relatedPanelProperties.autoAllignment = true;
		g->Panel("Directional light", -1080 + 200 + 5, 0, 400, 1080, glm::vec4(0.3, 0.3, 0.3, 0.8));
		g->xAlign = g->LEFT;
		g->Text("ID %d", true, 0, 0, 20, COLORS::White, 0, go->id);
		g->CheckBox("Enable", &go->Enable, 0, 0, 20);
		if (g->CollapsingHeader("Transform", 0, 0, COLORS::DarkGray * 0.7f)) {
			g->Text("Position");
			glm::vec3 pos = go->GetPosition();
			g->SliderFloat("X pos", &(pos.x), -50, 50, 0, 0, 200, 25);
			g->SliderFloat("Y pos", &(pos.y), -50, 50, 0, 0, 200, 25);
			g->SliderFloat("Z pos", &(pos.z), -50, 50, 0, 0, 200, 25);
			g->Text("Scale");
			glm::vec3 scale = go->GetScale();
			g->SliderFloat("X scale", &scale[0], -20, 50, 0, 0, 200, 25);
			g->SliderFloat("Y scale", &scale[1], -20, 50, 0, 0, 200, 25);
			g->SliderFloat("Z scale", &scale[2], -20, 50, 0, 0, 200, 25);
			g->Text("Rotation");
			g->SliderFloat("Pitch", &tr->rotation[0], -6.28, 6.28, 0, 0, 200, 25);
			g->SliderFloat("Yaw", &tr->rotation[1], -6.28, 6.28, 0, 0, 200, 25);
			g->SliderFloat("Roll", &tr->rotation[2], -6.28, 6.28, 0, 0, 200, 25);
			if (g->Button("Reset", 0, 0, 20, 100, 20, glm::vec4(0.5, 0.5, 0.5, 1))) {
				tr->rotation = glm::vec3(0, 0, 0);
				scale = glm::vec3(1, 1, 1);
				pos = glm::vec3(0, 0, 0);
			}
			tr->Scale(scale[0], scale[1], scale[2]);
			tr->Translate(pos.x, pos.y, pos.z);
			tr->RotateOnce(tr->rotation.x, tr->rotation.y, tr->rotation.z, true);
		}
		if (g->CollapsingHeader("Light Component", 0, 0, COLORS::DarkGray * 0.7f)) {
			g->SliderFloat("Intensity", &dirlight->intensity, 0, 10, 0, 0, 200, 25);
			g->Bar(0, 0, dirlight->intensity, 10,COLORS::Red,COLORS::White,200,25);
		}
		if (g->CollapsingHeader("Shadow map", 0, 0, COLORS::DarkGray * 0.7f)) {
			Texture* shadowMap = new Texture;
			shadowMap->m_RendererID = Window::GetCamera().frameBufferShadowMap->texture;
			glm::vec2 size{256,256};
			g->Image(size.x / 2, -size.y / 2, size.x, size.y, shadowMap);
			g->SliderFloat("Znear", &Window::GetCamera().znears, -500, 500, 0, 0, 200, 25);
			g->SliderFloat("Zfar", &Window::GetCamera().zfars, 0, 500, 0, 0, 200, 25);
			g->SliderFloat("Projection", &Window::GetCamera().rationprojections, 0, 1000, 0, 0, 200, 25);
			g->SliderFloat("DrawShadows", &Instancing::Instance()->drawShadows, 0, 1, 0, 0, 200, 25);
		}
		g->UnRelateToPanel();
	}
}
