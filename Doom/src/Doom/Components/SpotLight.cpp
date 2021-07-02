#include "../pch.h"
#include "SpotLight.h"

using namespace Doom;

void Doom::SpotLight::Copy(const SpotLight& rhs)
{
	m_Constant = rhs.m_Constant;
	m_Linear = rhs.m_Linear;
	m_Quadratic = rhs.m_Quadratic;
	m_InnerCutOff = rhs.m_InnerCutOff;
	m_OuterCutOff = rhs.m_OuterCutOff;
	m_Color = rhs.m_Color;
}

void Doom::SpotLight::operator=(const SpotLight& rhs)
{
	Copy(rhs);
}

Component* Doom::SpotLight::Create()
{
	return new SpotLight();
}

Doom::SpotLight::SpotLight(const SpotLight& rhs)
{
	Copy(rhs);
}

Doom::SpotLight::SpotLight()
{
	std::function<void()>* f = new std::function<void()>([=] {
		SpriteRenderer* sr = m_OwnerOfCom->GetComponent<SpriteRenderer>();
		if (sr == nullptr)
			sr = m_OwnerOfCom->AddComponent<SpriteRenderer>();
		sr->m_DisableRotation = true;
		sr->m_Texture = Texture::Get("src/UIimages/Flashlight.png");
		s_SpotLights.push_back(this);
		});
	EventSystem::GetInstance().SendEvent(EventType::ONMAINTHREADPROCESS, nullptr, f);
}

Doom::SpotLight::~SpotLight()
{
	auto iter = std::find(s_SpotLights.begin(), s_SpotLights.end(), this);
	if (iter != s_SpotLights.end())
		s_SpotLights.erase(iter);
}
