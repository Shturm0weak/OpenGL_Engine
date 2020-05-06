#include "../pch.h"
#include "Editor.h"
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

using namespace Doom;

void Editor::EditorUpdate()
{
	ImGui::Begin("Doom Engine", &tool_active, ImGuiWindowFlags_MenuBar);
	if (ImGui::CollapsingHeader("Scene")) {
		Renderer::CalculateObjectsVectors();
		if (ImGui::Button("Create a gameobject")) {
			Renderer::CreateGameObject();
			Renderer::CalculateObjectsVectors().size();
			selected = Renderer::GetAmountOfObjects() - Renderer::GetObjectsWithOwnerReference().size() - 1;
		}
		if (ImGui::CollapsingHeader("GameObjects")) {
			ImGui::ListBox("Objects", &selected, Renderer::GetItems(), Renderer::GetObjectsWithNoOwnerReference().size());
		}
		GameObject* go = nullptr;
		if (Renderer::GetObjectsWithNoOwnerReference().size() > 0) {
			go = static_cast<GameObject*>(Renderer::GetReference(Renderer::GetObjectsWithNoOwnerReference()[selected]));
			if (go == nullptr)
				return;
			Doom::Transform* tr = go->component_manager->GetComponent<Doom::Transform>();
			Doom::Collision* col = go->component_manager->GetComponent<Doom::Collision>();
			ImGui::Text("ID %d", go->GetId());
			ImGui::Checkbox("Enable", &go->Enable);
			ImGui::Checkbox("Static", &go->Static);
			if (go->GetOwner() != nullptr)
				ImGui::Text("Has Owner");
			ImGui::InputText("Name", name, sizeof(name));
			ImGui::SameLine();
			if (ImGui::Button("Change name")) {
				go->SetName(name);
			}
			ImGui::SliderInt("Layer", &go->GetLayer(), 0, Renderer::GetAmountOfObjects() - 1);
			if (ImGui::Button("Change layer")) {
				if (go->GetLayer() > Renderer::GetAmountOfObjects() - 1) {
					std::cout << "Error: layer out of range" << std::endl;
					return;
				}
				go->Setlayer(go->GetLayer());
				Renderer::CalculateObjectsVectors();
				for (unsigned int i = 0; i < Renderer::GetObjectsWithNoOwnerReference().size(); i++)
				{
					if (Renderer::GetObjectsWithNoOwnerReference()[i] == go->GetId()) {
						selected = i;
						break;
					}
					selected = 0;
				}
			}
			if (ImGui::Button("Clone gameobject")) {
				GameObject* clgo = Renderer::CreateGameObject();
				clgo->operator=(*go);
				selected = Renderer::GetObjectsWithNoOwnerReference().size();
			}
			if (ImGui::Button("Delete selected gameobject")) {
				if (selected == Renderer::GetObjectsWithNoOwnerReference().size() - 1)
					selected--;
				Renderer::DeleteObject(go->GetId());
				ImGui::End();
				//ImGui::Render();
				//ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
				return;
			}
			if (go->GetChilds().size() >  0) {
				if (ImGui::CollapsingHeader("Childs")) {
					ImGui::ListBox("Childs", &selectedchild, go->GetItemsNames(), go->GetChilds().size());
				}
			}
			if (selectedchild > -1) {
				GameObject* child = static_cast<GameObject*>(Renderer::GetReference(Renderer::GetObjectsWithOwnerReference()[selectedchild]));
				Doom::Transform* trchild = child->component_manager->GetComponent<Doom::Transform>();
				Doom::Collision* colchild = child->component_manager->GetComponent<Doom::Collision>();
				ImGui::Begin(child->name->c_str(), &tool_active);
				ImGui::Text("ID %d", child->GetId());
				if (ImGui::CollapsingHeader("Components")) {
					ImGui::ListBox("Components", &selectedcomponent, child->GetComponentManager()->GetItems(), child->GetComponentManager()->GetAmountOfComponents());
					if (colchild == nullptr) {
						if (ImGui::Button("Add collision")) {
							child->GetComponentManager()->AddComponent<Doom::Collision>();
						}
					}
				}
				if (ImGui::CollapsingHeader("Transform")) {
					ImGui::Text("Position: x: %lf y: %lf", child->GetPositions().x, child->GetPositions().y);
					ImGui::InputFloat2("Set the borders of X and Y position slider", changeSliderPos);
					ImGui::SliderFloat("Position X", &(trchild->position.x), changeSliderPos[0], changeSliderPos[1]);
					ImGui::SliderFloat("Position Y", &(trchild->position.y), changeSliderPos[0], changeSliderPos[1]);
					ImGui::InputFloat2("Positions", &(trchild->position.x, trchild->position.x));
					ImGui::Text("Scale");
					ImGui::InputFloat2("Set the borders of X and Y scale slider", changeSliderScale);
					ImGui::SliderFloat("Scale X", &(child->scaleValues[0]), changeSliderScale[0], changeSliderScale[1]);
					ImGui::SliderFloat("Scale Y", &(child->scaleValues[1]), changeSliderScale[0], changeSliderScale[1]);
					ImGui::InputFloat2("Scale", &(child->scaleValues[0], child->scaleValues[0]));
					color = child->GetColor();
					ImGui::ColorEdit4("Color", color);
					trchild->Scale(child->scaleValues[0], child->scaleValues[1]);
					if (child->GetComponentManager()->GetComponent<Animator>() == nullptr || child->GetComponentManager()->GetComponent<Animator>()->isPlayingAnim == false)
						child->SetColor(glm::vec4(color[0], color[1], color[2], color[3]));
					delete[] color;
					int counterImagesButtons = 0;
					for (unsigned int i = 0; i < texture.size(); i++)
					{
						void* my_tex_id = reinterpret_cast<void*>(texture[i]->m_RendererID);
						int frame_padding = -1;
						if (counterImagesButtons > 9) {
							ImGui::NewLine();
							counterImagesButtons = 0;
						}
						if (ImGui::ImageButton(my_tex_id, ImVec2(36, 36), ImVec2(1, 1), ImVec2(0, 0), frame_padding, ImVec4(1.0f, 1.0f, 1.0f, 0.5f))) {
							if (child != nullptr) {
								child->SetTexture(texturesPath[i]);
							}
						}
						ImGui::SameLine();
						counterImagesButtons++;
					}
					ImGui::NewLine();
					if (ImGui::Button("Nod texture")) {
						if (child != nullptr)
							child->SetTexture(nullptr);
					}
					ImGui::NewLine();
					if (ImGui::Button("Refresh textures")) {
						for (unsigned int i = 0; i < texture.size(); i++)
						{
							texture[i]->~Texture();
						}
						texturesPath.clear();
						texture.clear();
						CheckTexturesFolder("src/Images");
					}
					ImGui::SliderAngle("Rotate", &trchild->angle);
					ImGui::InputInt3("Rotate axes", axes);
					trchild->RotateOnce(trchild->angle, glm::vec3(axes[0], axes[1], axes[2]));
				}
				if (colchild != nullptr) {
					if (ImGui::CollapsingHeader("Collision")) {
						ImGui::Text("Collision");
						ImGui::Text("ID %d", colchild->GetId());
						ImGui::Text("Tag %s", colchild->GetTag().c_str());
						ImGui::InputText("Tag", tag, sizeof(tag));
						ImGui::SameLine();
						if (ImGui::Button("Change tag")) {
							colchild->SetTag(tag);
						}
						ImGui::Checkbox("Enable collision", &colchild->Enable);
						ImGui::Checkbox("Trigger", &colchild->IsTrigger);
						ImGui::InputFloat2("Set the borders of X and Y offset slider", changeSliderCollisionOffset);
						ImGui::SliderFloat("Offset X", &colchild->offsetX, changeSliderCollisionOffset[0], changeSliderCollisionOffset[1]);
						ImGui::SliderFloat("Offset Y", &colchild->offsetY, changeSliderCollisionOffset[0], changeSliderCollisionOffset[1]);
						ImGui::InputFloat2("Offset", &(colchild->offsetX, colchild->offsetX));
						colchild->SetOffset(colchild->offsetX, colchild->offsetY);
						if (ImGui::Button("Remove collision")) {
							child->component_manager->RemoveComponent<Doom::Collision>();
							selectedcomponent = 0;
						}
					}
				}
				if(ImGui::Button("Close")) {
					selectedchild = -1;
				}
				ImGui::End();
			}
			if (ImGui::CollapsingHeader("Components")) {
				ImGui::ListBox("Components",&selectedcomponent, go->GetComponentManager()->GetItems(), go->GetComponentManager()->GetAmountOfComponents());
				if (col == nullptr) {
					if (ImGui::Button("Add collision")) {
						go->GetComponentManager()->AddComponent<Doom::Collision>();
					}
				}
			}
			if (ImGui::CollapsingHeader("Transform")) {
				ImGui::Text("Position: x: %lf y: %lf", go->GetPositions().x, go->GetPositions().y);
				ImGui::InputFloat2("Set the borders of X and Y position slider", changeSliderPos);
				ImGui::SliderFloat("Position X", &(tr->position.x), changeSliderPos[0], changeSliderPos[1]);
				ImGui::SliderFloat("Position Y", &(tr->position.y), changeSliderPos[0], changeSliderPos[1]);
				ImGui::InputFloat2("Positions", &(tr->position.x, tr->position.x));
				ImGui::Text("Scale");
				ImGui::InputFloat2("Set the borders of X and Y scale slider", changeSliderScale);
				ImGui::SliderFloat("Scale X", &(go->scaleValues[0]), changeSliderScale[0], changeSliderScale[1]);
				ImGui::SliderFloat("Scale Y", &(go->scaleValues[1]), changeSliderScale[0], changeSliderScale[1]);
				ImGui::InputFloat2("Scale", &(go->scaleValues[0], go->scaleValues[0]));
				color = go->GetColor();
				ImGui::ColorEdit4("Color", color);
				tr->Scale(go->scaleValues[0], go->scaleValues[1]);
				if(go->GetComponentManager()->GetComponent<Animator>() == nullptr || go->GetComponentManager()->GetComponent<Animator>()->isPlayingAnim == false)
					go->SetColor(glm::vec4(color[0], color[1], color[2], color[3]));
				delete[] color;
				int counterImagesButtons = 0;
				ImGui::Text("Textures");
				for (unsigned int i = 0; i < texture.size(); i++)
				{
					void* my_tex_id = reinterpret_cast<void*>(texture[i]->m_RendererID);
					int frame_padding = -1;
					if (counterImagesButtons > 9) {
						ImGui::NewLine();
						counterImagesButtons = 0;
					}
					if (ImGui::ImageButton(my_tex_id, ImVec2(36, 36), ImVec2(1, 1), ImVec2(0, 0), frame_padding, ImVec4(1.0f, 1.0f, 1.0f, 0.5f))) {
						if (go != nullptr) {
							go->SetTexture(texture[i]);
						}
					}
					ImGui::SameLine();
					counterImagesButtons++;
				}

				ImGui::NewLine();
				ImGui::Text("Texture Atlases");
				int counterAtlasesButtons = 0;
				for (unsigned int i = 0; i < TextureAtlas::textureAtlases.size(); i++)
				{
					void* my_tex_id = reinterpret_cast<void*>(TextureAtlas::textureAtlases[i]->GetTexture()->m_RendererID);
					int frame_padding = -1;
					if (counterAtlasesButtons > 9) {
						ImGui::NewLine();
						counterAtlasesButtons = 0;
					}
					if (ImGui::ImageButton(my_tex_id, ImVec2(36, 36), ImVec2(1, 1), ImVec2(0, 0), frame_padding, ImVec4(1.0f, 1.0f, 1.0f, 0.5f))) {
						if (go != nullptr) {
							go->textureAtlas = TextureAtlas::textureAtlases[i];
							go->SetTexture(go->textureAtlas->GetTexture());
						}
					}
					ImGui::SameLine();
					counterAtlasesButtons++;
				}

				ImGui::NewLine();
				ImGui::InputFloat2("UVs Offset",uvsOffset);
				
				if (ImGui::Button("Use these UVs")) {
					if (go != nullptr && go->textureAtlas != nullptr)
						go->SetUVs(go->textureAtlas->GetSpriteUVs(uvsOffset[0],uvsOffset[1]));
				}
				
				if (ImGui::Button("No texture")) {
					if (go != nullptr)
						go->SetTexture(nullptr);
				}
				if (ImGui::Button("Refresh textures")) {
					for (unsigned int i = 0; i < texture.size(); i++)
					{
						texture[i]->~Texture();
					}
					texturesPath.clear();
					texture.clear();
					CheckTexturesFolder("src/Images");
				}
				ImGui::SliderAngle("Rotate", &tr->angle);
				ImGui::InputInt3("Rotate axes",axes);
				tr->RotateOnce(tr->angle, glm::vec3(axes[0], axes[1], axes[2]));
			}
			if (col != nullptr) {
				if (ImGui::CollapsingHeader("Collision")) {
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
					ImGui::SliderFloat("Offset X", &col->offsetX, changeSliderCollisionOffset[0], changeSliderCollisionOffset[1]);
					ImGui::SliderFloat("Offset Y", &col->offsetY, changeSliderCollisionOffset[0], changeSliderCollisionOffset[1]);
					ImGui::InputFloat2("Offset", &(col->offsetX, col->offsetX));
					col->SetOffset(col->offsetX, col->offsetY);
					if (ImGui::Button("Remove collision")) {
						go->component_manager->RemoveComponent<Doom::Collision>();
						selectedcomponent = 0;
					}
				}
			}
			if (go->GetComponentManager()->GetComponent<Animator>() != nullptr) {
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
			ImGui::Checkbox("Visible collisions", &Doom::Collision::IsVisible);
			ImGui::SliderFloat("Zoom", &Window::GetCamera().zoomlevel, 0.1f, 1000.f);
			Window::GetCamera().Zoom(abs(Window::GetCamera().GetZoomLevel()));
			tr->Translate(tr->position.x, tr->position.y);
		}
		if (ImGui::Button("Save")) {
			Renderer::Save("C:/Users/Alexandr/Desktop/saved.txt");
		}
		ImGui::SameLine();
		if (ImGui::Button("Load")) {
			selected = 0;
			Renderer::Load("C:/Users/Alexandr/Desktop/saved.txt");
		}
		ImGui::SameLine();
		if (ImGui::Button("EXIT")) {
			glfwSetWindowShouldClose(Window::GetWindow(), GLFW_TRUE);
		}
	}
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS) , Draw calls %d", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate, Renderer::DrawCalls);
	ImGui::End();
}

glm::vec4 Editor::ColorPickUp(float* c) {
	
	bool tool_active = true;
	ImGui::Begin("Pick Color", &tool_active, ImGuiWindowFlags_MenuBar);
	ImGui::ColorEdit4("Color", c);
	glm::vec4 color = glm::vec4(c[0],c[1],c[2],c[3]);
	ImGui::End();

	
	return color;
}

void Editor::CheckTexturesFolder(const std::string path)
{
	auto f = std::bind([](std::string path) {
		for (const auto & entry : fs::directory_iterator(path)) {
			texturesPath.push_back(entry.path().string());
			size_t index = 0;
			index = texturesPath.back().find("\\", index);
			texturesPath.back().replace(index, 1, "/");
			texture.push_back(new Texture(texturesPath.back(),true));
		}
		for (unsigned int i = 0; i < texture.size(); i++)
		{
			std::function<void()> f2 = std::bind(&Texture::GenTexture,texture[i]);
			std::function<void()>* f1 = new std::function<void()>(f2);
			EventSystem::Instance()->SendEvent("OnMainThreadProcess",nullptr,f1);
		}
		
		
	}, path);
	ThreadPool::Instance()->enqueue(f);

	
}

Editor * Editor::Instance()
{
	static Editor instance;
	return &instance;
}