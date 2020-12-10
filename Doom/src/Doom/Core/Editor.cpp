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
	MenuInstancingStats();
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

	ImGui::SliderFloat("DrawShadows", &Instancing::Instance()->m_DrawShadows, 0, 1);

	CreateTextureAtlas();

	ImGui::SliderFloat("Zoom", &Window::GetCamera().m_ZoomLevel, 0.1f, 100.f);
	Window::GetCamera().Zoom(abs(Window::GetCamera().GetZoomLevel()));
	ImGui::End();

	ImGui::Begin("Scene");
	Editor::GetInstance()->MenuBar();
	if (ImGui::BeginPopupContextWindow())
	{
		if (ImGui::MenuItem("Create GameObject"))
		{
			World::CreateGameObject();
			go = World::s_GameObjects.back();
		}
		if (ImGui::MenuItem("Create 2D GameObject"))
		{
			World::CreateGameObject();
			go = World::s_GameObjects.back();
			go->GetComponentManager()->AddComponent<SpriteRenderer>();
		}
		if (ImGui::MenuItem("Create 3D GameObject"))
		{
			World::CreateGameObject();
			go = World::s_GameObjects.back();
			go->GetComponentManager()->AddComponent<Renderer3D>()->LoadMesh(MeshManager::GetMesh("cube"));
		}
		if (ImGui::MenuItem("Clone"))
		{
			GameObject* clgo = World::CreateGameObject();
			clgo->operator=(*go);
			go = clgo;
		}
			
		if (ImGui::MenuItem("Delete"))
		{
			if (go != nullptr) {
				World::DeleteObject(go->m_Id);
				if(World::s_GameObjects.size() > 0)
					go = World::s_GameObjects[World::s_GameObjects.size() - 1];
				else
					go = nullptr;
			}
			ImGui::EndPopup();
			ImGui::End();
			return;
		}
		ImGui::EndPopup();
	}

	ImGui::SetWindowFontScale(1.25);

	if (World::s_GameObjects.size() > 0 && go == nullptr) {
		go = World::s_GameObjects[0];
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

				GameObject* obj = World::CreateGameObject();
				obj->SetOwner((void*)go);
				go->AddChild((void*)obj);
				go = World::s_GameObjects.back();
				go->GetComponentManager()->AddComponent<Renderer3D>()->LoadMesh(MeshManager::GetMesh("cube"));
			}
			ImGui::EndPopup();
		}
		ImGui::SetWindowFontScale(1.25);
		if (go == nullptr) {
			ImGui::End();
			return;
		}
		Doom::Transform* tr = go->m_ComponentManager->GetComponent<Doom::Transform>();
		Doom::RectangleCollider2D* col = go->m_ComponentManager->GetComponent<Doom::RectangleCollider2D>();
		ImGui::Text("ID %d", go->m_Id);
		ImGui::Checkbox("Enable", &go->m_Enable);
		if (go->GetOwner() != nullptr)
			ImGui::Text("Has Owner");
		ImGui::InputText("Name", name, sizeof(name));
		ImGui::SameLine();
		if (ImGui::Button("Change name")) {
			go->m_Name = name;
		}
		ImGui::SliderInt("Layer", &go->GetLayer(), 0, World::GetAmountOfObjects() - 1);
		if (go->GetComponentManager()->GetComponent<SpriteRenderer>() != nullptr && go->GetComponentManager()->GetComponent<SpriteRenderer>()->m_RenderType == TYPE_2D) {
			if (ImGui::Button("Change layer")) {
				if (go->GetLayer() > World::GetAmountOfObjects() - 1) {
					std::cout << "Error: layer out of range" << std::endl;
					return;
				}
				go->GetComponent<SpriteRenderer>()->Setlayer(go->GetLayer());
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
		MenuShadowMap();
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
	for (auto i = Texture::s_Textures.begin(); i != Texture::s_Textures.end();i++)
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
	if (go->GetComponentManager()->GetComponent<Renderer3D>() != nullptr && go->GetComponentManager()->GetComponent<Renderer3D>()->m_RenderType == TYPE_3D) {
		if (MenuRemoveComponent<Renderer3D>()) {
			if (ImGui::CollapsingHeader("Renderer 3D")) {
				Renderer3D* r = go->GetComponentManager()->GetComponent<Renderer3D>();
				ImGui::Indent(ImGui::GetWindowSize().x * 0.05);
				ImGui::Checkbox("Cast shadows", &r->m_IsCastingShadows);
				ImGui::Checkbox("Wire mesh", &r->m_IsWireMesh);
				ImGui::Checkbox("Is culling face", &r->m_IsCullingFace);
				if (ImGui::CollapsingHeader("Shader")) {
					Shader* shader = go->GetComponentManager()->GetComponent<Renderer3D>()->m_Shader;
					ImGui::Text("%s", shader->m_Name);
					if (ImGui::Button("Reload")) {
						shader->Reload();
					}
					ImGui::InputText("Shader's name", this->name, sizeof(this->name));
					if (ImGui::Button("Change shader")) {
						Shader* shaderTemp = Shader::Get(this->name);
						if (shaderTemp != nullptr) {
							r->m_Shader = shaderTemp;
						}
					}
				}
				if (ImGui::CollapsingHeader("Material")) {
					if (!r->m_IsTransparent) {
						if (ImGui::Button("Make Transparent")) {
							r->MakeTransparent();
						}
					}
					else {
						if (ImGui::Button("Make Solid")) {
							r->MakeSolid();
						}
					}

					if (r->m_RenderTechnic == Renderer3D::RenderTechnic::Forward) {
						if (ImGui::Button("Push in Instance rendering")) {
							r->ChangeRenderTechnic(Renderer3D::RenderTechnic::Instancing);
						}
					}
					else {
						if (ImGui::Button("Push in Forward rendering")) {
							r->ChangeRenderTechnic(Renderer3D::RenderTechnic::Forward);
						}
					}

					ImGui::SliderFloat("Ambient", &r->m_Material.m_Ambient, 0, 1);
					ImGui::SliderFloat("Specular", &r->m_Material.m_Specular, 0, 50);
					void* my_tex_id = reinterpret_cast<void*>(r->m_DiffuseTexture->m_RendererID);
					if (ImGui::ImageButton(my_tex_id, { 64,64 }, ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0.79, 0, 0.75, 1))) {
						isActiveTexturePicker = true;
						texturePickerId = 1;
					}
					if (r->m_NormalMapTexture == nullptr)
						my_tex_id = reinterpret_cast<void*>(Texture::Get("InvalidTexture")->m_RendererID);
					else
						my_tex_id = reinterpret_cast<void*>(r->m_NormalMapTexture->m_RendererID);
					if (ImGui::ImageButton(my_tex_id, { 64,64 }, ImVec2(0, 0), ImVec2(1, 1), -1, ImVec4(0.79, 0, 0.75, 1))) {
						isActiveTexturePicker = true;
						texturePickerId = 2;
					}
					ImGui::Checkbox("NormalMap", &r->m_IsUsingNormalMap);
					float* tempColor = r->GetColor();
					ImGui::ColorEdit4("Color", tempColor);
					r->SetColor(glm::vec4(tempColor[0], tempColor[1], tempColor[2], tempColor[3]));
				}
				if (ImGui::CollapsingHeader("Mesh")) {
					if (r->m_Mesh != nullptr) {
						ImGui::Text("Name: %s", r->m_Mesh->m_Name);
						ImGui::Text("Id: %i", r->m_Mesh->m_IdOfMeshInFile);
						glm::vec2 minP = r->m_Owner->GetComponent<CubeCollider3D>()->m_MinP;
						glm::vec2 maxP = r->m_Owner->GetComponent<CubeCollider3D>()->m_MaxP;
						ImGui::Text("MinP: %f %f", minP.x, minP.y);
						ImGui::Text("MaxP: %f %f", maxP.x, maxP.y);
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
	if (cc != nullptr && cc->m_IsBoundingBox == false) {
		if (MenuRemoveComponent<CubeCollider3D>()) {
			if (ImGui::CollapsingHeader("CubeCollider3D")) {
				ImGui::SliderFloat3("Position", &cc->m_Offset.x, -50, 50);
			}
		}
	}
}

void Doom::Editor::MenuRenderer2D()
{
	if (go->GetComponentManager()->GetComponent<SpriteRenderer>() != nullptr && go->GetComponentManager()->GetComponent<SpriteRenderer>()->m_RenderType == TYPE_2D) {
		SpriteRenderer* sr = go->GetComponent<SpriteRenderer>();
		if (MenuRemoveComponent<SpriteRenderer>()) {
			if (ImGui::CollapsingHeader("Render2D")) {
				color = sr->GetColor();
				ImGui::ColorEdit4("Sprite color", color);
				sr->SetColor(glm::vec4(color[0], color[1], color[2], color[3]));
				delete[] color;
				int counterImagesButtons = 0;
				ImGui::Text("Textures");
				for (auto i = Texture::s_Textures.begin(); i != Texture::s_Textures.end(); i++)
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
					if (go != nullptr && sr->m_TextureAtlas != nullptr)
						sr->SetUVs(sr->m_TextureAtlas->GetSpriteUVs(uvsOffset[0], uvsOffset[1]));
				}
				if (ImGui::Button("Original UVs")) {
					if (go != nullptr)
						sr->OriginalUvs();
				}
				if (ImGui::Button("No texture")) {
					if (go != nullptr)
						sr->SetTexture(Texture::s_WhiteTexture);
				}
				if (ImGui::Button("Load texture ...")) {
					std::optional<std::string> filePath = FileDialogs::OpenFile("textures (*.png)\0");
					if (filePath) {
						Texture::AsyncLoadTexture(*filePath);
					}
				}
				int prevselectedAtlas = selectedAtlas;
				if (TextureAtlas::s_TextureAtlases.size() > 0) {
					ImGui::ListBox("Texture atlases", &selectedAtlas, TextureAtlas::GetTextureAtlases(), TextureAtlas::s_TextureAtlases.size());
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
										sr->m_TextureAtlas = TextureAtlas::GetTextureAtlas(selectedAtlas);
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
					go->m_ComponentManager->RemoveComponent<Doom::RectangleCollider2D>();
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
				ImGui::Text("counter %d", anim->m_Counter);
				ImGui::ListBox("Animations", &selectedanimation, anim->GetAnimations(), anim->GetAmountOfAnimations());

				int count = 0;
				ImGui::SliderFloat("Animation speed slider", &anim->m_Speed, 0, 100);
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
					anim->m_SelectedAnim = selectedanimation;
				}
				ImGui::NewLine();
				ImGui::Checkbox("Play animation", &anim->m_IsPlayingAnim);
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
		ImGui::SliderAngle("Pitch", &tr->m_Rotation.x);
		ImGui::SliderAngle("Yaw", &tr->m_Rotation.y);
		ImGui::SliderAngle("Roll", &tr->m_Rotation.z);
		tr->Scale(scale[0], scale[1], scale[2]);
		tr->Translate(position.x, position.y, position.z);
		tr->RotateOnce(tr->m_Rotation.x, tr->m_Rotation.y, tr->m_Rotation.z, true);
	}
}

void Doom::Editor::MenuLightPoint()
{
	PointLight* pl = go->GetComponentManager()->GetComponent<PointLight>();
	if (pl == nullptr)
		return;
	if (MenuRemoveComponent<PointLight>()) {
		if (ImGui::CollapsingHeader("Point light")) {
			ImGui::SliderFloat("Constant", &pl->m_Constant, 0, 1);
			ImGui::SliderFloat("Linear", &pl->m_Linear, 0, 0.100f);
			ImGui::SliderFloat("Quadratic", &pl->m_Quadratic, 0, 0.100);
			ImGui::ColorPicker3("Point light color", &pl->m_Color[0]);
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
			ImGui::ColorPicker3("Dir light color", &pl->m_Color[0]);
			ImGui::SliderFloat("Intensity", &pl->m_Intensity, 1, 10);
		}
	}
}

void Doom::Editor::MenuSphereCollisionComponent()
{
	if (go->GetComponentManager()->GetComponent<SphereCollider>() != nullptr) {
		if (MenuRemoveComponent<SphereCollider>()) {
			SphereCollider* cs = go->GetComponentManager()->GetComponent<SphereCollider>();
			if (ImGui::CollapsingHeader("Sphere collider")) {
				ImGui::Checkbox("Is in bounding box", &cs->m_IsInBoundingBox);
				ImGui::SliderFloat("Radius", &cs->m_Radius, 0, 50);
				ImGui::SliderFloat2("Offset", &cs->m_Offset[0], -100, 100);
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
			World::ShutDown();
			go = nullptr;
		}
		if (ImGui::MenuItem("Open")) {
			std::optional<std::string> filePath = FileDialogs::OpenFile("Doom Scene (*.yaml)\0*.yaml\0");
			if (filePath) {
				World::ShutDown();
				SceneSerializer::DeSerialize(*filePath);
				go = nullptr;
			}
		}
		if (ImGui::MenuItem("Save as")) {
			std::optional<std::string> filePath = FileDialogs::SaveFile("Doom Scene (*.yaml)\0*.yaml\0");
			if (filePath) {
				SceneSerializer::Serialize(*filePath);
			}
		}
		if (ImGui::MenuItem("Save")) {
			SceneSerializer::Serialize(SceneSerializer::s_CurrentSceneFilePath);
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
		auto mesh = MeshManager::s_Meshes.begin();
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
	if (ImGui::Button("Load fbx scene ...")) {
		std::optional<std::string> filePath = FileDialogs::OpenFile("files (*.fbx)\0");
		if (filePath) {
			MeshManager::LoadScene(*filePath);
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
	for (auto texture : Texture::s_Textures)
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
				go->GetComponent<Renderer3D>()->m_DiffuseTexture = (texture.second);
				break;
			case 2:
				go->GetComponent<Renderer3D>()->m_NormalMapTexture = (texture.second);
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
		for (uint32_t i = 0; i < World::GetAmountOfObjects(); i++)
		{
			GameObject* go = World::s_GameObjects[i];
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
	bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)go, flags, go->m_Name.c_str());
	if (ImGui::IsItemClicked()) {
		if(this->go != nullptr)
			this->go = go;
	}

	if (opened) {
		ImGui::TreePop();
		DrawChilds(go);
	}
}

void Doom::Editor::DrawChilds(GameObject * go)
{
	for (uint32_t i = 0; i < go->m_Childs.size(); i++)
	{
		GameObject* child = static_cast<GameObject*>(go->m_Childs[i]);
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
	ImGui::ListBox("Shaders", &selectedShader, Shader::GetListOfShaders(), Shader::s_Shaders.size());
	if (ImGui::Button("Reload")) {
		auto shader = Shader::s_Shaders.begin();
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

void Doom::Editor::MenuShadowMap()
{
	void* my_tex_id = reinterpret_cast<void*>(Window::GetCamera().m_FrameBufferShadowMap->m_Texture);
	ImGui::Begin("FrameBuffer");
	ImGui::Image(my_tex_id, ImVec2(512, 512), ImVec2(0, 1), ImVec2(1, 0));
	ImGui::SliderFloat("Znear", &Window::GetCamera().m_ZnearSM, -500, 500);
	ImGui::SliderFloat("Zfar", &Window::GetCamera().m_ZfarSM, 0, 500);
	ImGui::SliderFloat("Projection", &Window::GetCamera().m_RationProjectionSM, 0, 1000);
	ImGui::SliderFloat("DrawShadows", &Instancing::Instance()->m_DrawShadows, 0, 1);
	ImGui::End();
}

void Doom::Editor::MenuInstancingStats()
{
	ImGui::Begin("Instancing stats");
	ImGui::Text("Meshes in instancing %i", Instancing::Instance()->m_InstancedObjects.size());
	for (auto iter = Instancing::Instance()->m_InstancedObjects.begin(); iter != Instancing::Instance()->m_InstancedObjects.end(); iter++)
	{
		ImGui::Text("%s : %i", iter->first->m_Name, iter->second.size());
	}
	ImGui::End();
}

void Doom::Editor::ShortCuts()
{
	if (!Input::IsMouseDown(Keycode::MOUSE_BUTTON_2)) {
		if (Input::IsKeyPressed(Keycode::KEY_W)) {
			ViewPort::GetInstance()->m_GizmoOperation = 0;
		}
		else if (Input::IsKeyPressed(Keycode::KEY_R)) {
			ViewPort::GetInstance()->m_GizmoOperation = 1;
		}
		else if (Input::IsKeyPressed(Keycode::KEY_S)) {
			ViewPort::GetInstance()->m_GizmoOperation = 2;
		}
		else if (Input::IsKeyPressed(Keycode::KEY_Q)) {
			ViewPort::GetInstance()->m_GizmoOperation = -1;
		}
		if (Input::IsKeyPressed(Keycode::KEY_BACKSPACE)) {
			if (Editor::GetInstance()->go != nullptr) {
				World::DeleteObject(Editor::GetInstance()->go->m_Id);
				if (World::s_GameObjects.size() > 0)
					Editor::GetInstance()->go = World::s_GameObjects[World::s_GameObjects.size() - 1];
				else
					Editor::GetInstance()->go = nullptr;
			}
		}
		if (Input::IsKeyDown(Keycode::KEY_LEFT_CONTROL) && Input::IsKeyPressed(Keycode::KEY_C)) {
			copiedGo = go;
		}
		else if (Input::IsKeyDown(Keycode::KEY_LEFT_CONTROL) && Input::IsKeyPressed(Keycode::KEY_V)) {
			if (copiedGo != nullptr) {
				World::CreateGameObject()->operator=(*copiedGo);
				go = World::s_GameObjects.back();
			}
		}
	}
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
		for (const auto & entry : fs::directory_iterator(path)) {
			std::string pathToTexture = entry.path().string();
			if (pathToTexture.find(".png") <= pathToTexture.length() || pathToTexture.find(".jpeg") <= pathToTexture.length()) {
				s_TexturesPath.push_back(pathToTexture);
				size_t index = 0;
				index = s_TexturesPath.back().find("\\", index);
				s_TexturesPath.back().replace(index, 1, "/");
				Texture::AsyncLoadTexture(s_TexturesPath.back());
				Texture::GetAsync(&s_TexturesPath.back(), [=] {
					Texture* t = Texture::Get(s_TexturesPath.back());
					if (t != nullptr)
						s_Texture.push_back(t);
					return t;
					});
			}
		}
	}, path);
	ThreadPool::GetInstance()->Enqueue(f);
}

void Doom::Editor::CheckTexturesFolder(const std::string path)
{
	auto f = std::bind([](std::string path) {
		for (const auto& entry : fs::directory_iterator(path)) {
			std::string pathToTexture = entry.path().string();
			if (pathToTexture.find(".png") <= pathToTexture.length() || pathToTexture.find(".jpeg") <= pathToTexture.length()) {
				s_TexturesPath.push_back(pathToTexture);
				size_t index = 0;
				index = s_TexturesPath.back().find("\\", index);
				s_TexturesPath.back().replace(index, 1, "/");
				Texture::AsyncLoadTexture(s_TexturesPath.back());
				Texture::GetAsync(&s_TexturesPath.back(), [=] {
					Texture* t = Texture::Get(s_TexturesPath.back(), false);
					if (t != nullptr)
						s_Texture.push_back(t);
					return t;
					});
			}
		}
		}, path);
	ThreadPool::GetInstance()->Enqueue(f);
}

void Doom::Editor::Debug()
{
	ImGui::Begin("Camera");
	ImGui::SliderAngle("X", &Window::GetCamera().m_Pitch);
	ImGui::SliderAngle("Y", &Window::GetCamera().m_Yaw);
	ImGui::SliderAngle("Z", &Window::GetCamera().m_Roll);
	if (Window::GetCamera().m_Type == Window::GetCamera().PERSPECTIVE) {
		ImGui::SliderAngle("fov", &Window::GetCamera().m_Fov, 60, 180);
		Window::GetCamera().SetFov(Window::GetCamera().m_Fov);
	}
	ImGui::Text("Position");
	ImGui::Text("x: %f y: %f z: %f", Window::GetCamera().GetPosition().x, Window::GetCamera().GetPosition().y, Window::GetCamera().GetPosition().z);
	ImGui::Text("Forward vector");
	ImGui::Text("x: %f y: %f z: %f", Window::GetCamera().forwardV.x, Window::GetCamera().forwardV.y, Window::GetCamera().forwardV.z);
	Window::GetCamera().SetRotation(glm::vec3(Window::GetCamera().m_Pitch, Window::GetCamera().m_Yaw, Window::GetCamera().m_Roll));
	ImGui::End();
	ImGui::Begin("Debug");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::Text("Objects amount %d", World::s_GameObjects.size());
	ImGui::Text("Draw calls %d", Renderer::s_DrawCalls);
	ImGui::Text("Vertices %d", Renderer::s_Vertices);
	ImGui::Text("Textures amount %d", Texture::s_Textures.size());
	ImGui::Checkbox("Polygon mode",&Renderer::s_PolygonMode);
	ImGui::Checkbox("Visible collisions", &RectangleCollider2D::s_IsVisible);
	ImGui::Checkbox("Visible bounding boxes", &isBoundingBoxesVisible);
	ImGui::End();
	TextProps();
}

void Doom::Editor::Threads() {
	ImGui::Begin("Threads");
	for (auto i = ThreadPool::GetInstance()->m_IsThreadBusy.begin(); i != ThreadPool::GetInstance()->m_IsThreadBusy.end(); i++)
	{
		ImGui::Text("ID: %d task: %d",i->first,i->second);
	}
	ImGui::End();
}

void Doom::Editor::TextProps()
{
	Gui* g = Gui::GetInstance();
	ImGui::Begin("Text properties");
	ImGui::SliderFloat("Width", &g->m_TextProps.m_Width,0,0.9);
	ImGui::SliderFloat("Edge", &g->m_TextProps.m_Edge, 0.01, 0.9);
	ImGui::SliderFloat("Border width", &g->m_TextProps.m_BorderWidth, 0, 0.9);
	ImGui::SliderFloat("Border edge", &g->m_TextProps.m_BorderEdge, 0.01, 0.9);
	ImGui::ColorPicker4("Out line color", TextColor);
	g->m_TextProps.m_OutLineColor = glm::vec4(TextColor[0], TextColor[1], TextColor[2], TextColor[3]);
	ImGui::SliderFloat2("Shaddow offset", shadowOffset,-0.01,0.01);
	g->m_TextProps.m_ShadowOffset = glm::vec2(shadowOffset[0], shadowOffset[1]);
	ImGui::End();
}

void Doom::Editor::UpdateNormals()
{
	std::lock_guard<std::mutex> lock(mtx_updateNormals);
	if (drawNormals) {
		uint32_t counter = 0;
		size_t size = World::s_GameObjects.size();
		for (size_t i = 0; i < size; i++)
		{
			Irenderer* iR = World::s_GameObjects[i]->GetComponent<Irenderer>();
			if (iR->m_RenderType == TYPE_3D) {
				Renderer3D* r = static_cast<Renderer3D*>(iR);
				Mesh* mesh = r->m_Mesh;
				size_t meshSize = r->m_Mesh->m_VertAttribSize;
				for (size_t j = 0; j < meshSize; j += 14)
				{
					glm::vec3 pos = glm::vec3(mesh->m_VertAttrib[j + 0], mesh->m_VertAttrib[j + 1], mesh->m_VertAttrib[j + 2]);
					glm::vec3 normals = glm::vec3(mesh->m_VertAttrib[j + 3], mesh->m_VertAttrib[j + 4], mesh->m_VertAttrib[j + 5]);
					glm::mat4 scaleXview = r->GetOwnerOfComponent()->GetComponent<Transform>()->m_ScaleMat4 * r->GetOwnerOfComponent()->GetComponent<Transform>()->m_ViewMat4;
					glm::vec4 transformedPos = scaleXview * glm::vec4(pos.x, pos.y, pos.z, 0);
					glm::vec4 transformedNor = scaleXview * glm::vec4(normals.x, normals.y, normals.z, 0);
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
	DirectionalLight* dirlight = DirectionalLight::s_DirLights[0];
	GameObject* go = dirlight->GetOwnerOfComponent();
	Transform* tr = go->GetComponent<Transform>();
	if (go != nullptr) {
		g->m_XAlign = Gui::AlignHorizontally::XCENTER;
		g->Text("FPS: %f", true, 900, 500, 40, COLORS::White, 0, Window::GetFPS());

		g->FontBind(Gui::GetInstance()->GetStandartFonts()[Gui::GetInstance()->ARIAL]);
		g->RelateToPanel();
		g->m_RelatedPanelProperties.m_AutoAllignment = true;
		g->Panel("Directional light", -1080 + 200 + 5, 0, 400, 1080, glm::vec4(0.3, 0.3, 0.3, 0.8));
		g->m_XAlign = g->LEFT;
		g->Text("ID %d", true, 0, 0, 20, COLORS::White, 0, go->m_Id);
		g->CheckBox("Enable", &go->m_Enable, 0, 0, 20);
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
			g->SliderFloat("Pitch", &tr->m_Rotation[0], -6.28, 6.28, 0, 0, 200, 25);
			g->SliderFloat("Yaw", &tr->m_Rotation[1], -6.28, 6.28, 0, 0, 200, 25);
			g->SliderFloat("Roll", &tr->m_Rotation[2], -6.28, 6.28, 0, 0, 200, 25);
			if (g->Button("Reset", 0, 0, 20, 100, 20, glm::vec4(0.5, 0.5, 0.5, 1))) {
				tr->m_Rotation = glm::vec3(0, 0, 0);
				scale = glm::vec3(1, 1, 1);
				pos = glm::vec3(0, 0, 0);
			}
			tr->Scale(scale[0], scale[1], scale[2]);
			tr->Translate(pos.x, pos.y, pos.z);
			tr->RotateOnce(tr->m_Rotation.x, tr->m_Rotation.y, tr->m_Rotation.z, true);
		}
		if (g->CollapsingHeader("Light Component", 0, 0, COLORS::DarkGray * 0.7f)) {
			g->SliderFloat("Intensity", &dirlight->m_Intensity, 0, 10, 0, 0, 200, 25);
			g->Bar(0, 0, dirlight->m_Intensity, 10,COLORS::Red,COLORS::White,200,25);
		}
		if (g->CollapsingHeader("Shadow map", 0, 0, COLORS::DarkGray * 0.7f)) {
			Texture* shadowMap = new Texture;
			shadowMap->m_RendererID = Window::GetCamera().m_FrameBufferShadowMap->m_Texture;
			glm::vec2 size{256,256};
			g->Image(size.x / 2, -size.y / 2, size.x, size.y, shadowMap);
			g->SliderFloat("Znear", &Window::GetCamera().m_ZnearSM, -500, 500, 0, 0, 200, 25);
			g->SliderFloat("Zfar", &Window::GetCamera().m_ZfarSM, 0, 500, 0, 0, 200, 25);
			g->SliderFloat("Projection", &Window::GetCamera().m_RationProjectionSM, 0, 1000, 0, 0, 200, 25);
			g->SliderFloat("DrawShadows", &Instancing::Instance()->m_DrawShadows, 0, 1, 0, 0, 200, 25);
		}
		g->UnRelateToPanel();
	}
}
