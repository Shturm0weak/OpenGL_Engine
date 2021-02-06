#include "Animator.h"
#include "../pch.h"
#include <filesystem>

namespace fs = std::filesystem;

void Doom::Animator::SetAnimation(const std::string path)
{
	for (const auto & _entry : fs::directory_iterator(path)) {
		size_t _index = 0;
		std::vector<Texture*> text;
		for (const auto & entry : fs::directory_iterator(_entry.path().string())) {
			m_TexturesPath.push_back(entry.path().string());
			size_t index = 0;
			for (int i = 0; i < 2; i++)
			{
				index = m_TexturesPath.back().find("\\", index);
				m_TexturesPath.back().replace(index, 1, "/");
			}
			text.push_back(Texture::Create(m_TexturesPath.back(), 1));
		}
		m_Amount++;
		animations.insert(std::make_pair(_entry.path().string(),text));
		m_TexturesPath.clear();
	}
}

void Doom::Animator::PlayAnim(std::vector<Texture*>& text) 
{
	if (m_IsPlayingAnim == false) return;
	m_Timer += DeltaTime::GetDeltaTime();
	m_Counter = m_Timer * m_Speed;
	if (m_Counter < text.size())
		m_OwnerOfCom->GetComponent<SpriteRenderer>()->m_Texture = (text[m_Counter]);
	if (m_Counter >= text.size())
		m_Timer = 0;
}

const char** Doom::Animator::GetAnimations()
{
	if(items != nullptr) 
		delete[] items;
	items = new const char*[m_Amount];
	int i = 0;
	for (auto itr = animations.begin(); itr != animations.end(); itr++)
	{
		items[i] = itr->first.c_str();
		i++;
	}
	return items;
}

Doom::Animator::Animator()
{
//	SetType(ComponentType::ANIMATOR);
}

Doom::Animator::~Animator()
{
	delete items;
}

Doom::Component* Doom::Animator::Create()
{
	return new Animator();
}
