#include "Animator.h"
#include "../pch.h"
#include <filesystem>

namespace fs = std::filesystem;

void Doom::Animator::SetAnimation(const std::string path)
{
	for (const auto & _entry : fs::directory_iterator(path)){
		size_t _index = 0;
		std::vector<Texture*> text;
		for (const auto & entry : fs::directory_iterator(_entry.path().string())) {
			texturesPath.push_back(entry.path().string());
			size_t index = 0;
			for (int i = 0; i < 2; i++)
			{
				index = texturesPath.back().find("\\", index);
				texturesPath.back().replace(index, 1, "/");	
			}
			text.push_back(new Texture(texturesPath.back(), 1));
		}
		amount++;
		animations.insert(std::make_pair(_entry.path().string(),text));
		texturesPath.clear();
	}
}

void Doom::Animator::PlayAnim(std::vector<Texture*>& text) {
	if (isPlayingAnim == false)
		return;
	timer += DeltaTime::GetDeltaTime();
	counter = timer * speed;
	if (counter < text.size()) {
		owner->GetComponentManager()->GetComponent<SpriteRenderer>()->SetTexture(text[counter]);
		//if (owner->ShaderType::SHADER_TEXTURE != owner->shadertype)
		//	owner->SetShader(GameObject::SHADER_TEXTURE);
		//owner->shader->Bind();
		//owner->texture->Bind(owner->texture->m_RendererID);
		//owner->shader->SetUniform1i("u_Texture", owner->texture->m_RendererID);
	}
	if (counter >= text.size())
		timer = 0;
}

int Doom::Animator::GetAmountOfAnimations() {
	return amount;
}

const char** Doom::Animator::GetAnimations() {
	if(items != nullptr)
	delete[] items;
	items = new const char*[amount];
	int i = 0;
	for (auto itr = animations.begin(); itr != animations.end(); itr++){
		items[i] = itr->first.c_str();
		i++;
	}
	return items;
}

Doom::Animator::Animator() {
	SetType("Animator");
}
