#include "../pch.h"
#include "PointLight.h"

using namespace Doom;

void Doom::PointLight::Copy(const PointLight& rhs)
{
	m_Constant = rhs.m_Constant;
	m_Linear = rhs.m_Linear;
	m_Quadratic = rhs.m_Quadratic;
	m_Color = rhs.m_Color;
}

void Doom::PointLight::operator=(const PointLight& rhs)
{
	Copy(rhs);
}

Component* Doom::PointLight::Create()
{
	return new PointLight();
}

Doom::PointLight::PointLight(const PointLight& rhs)
{
	Copy(rhs);
}

Doom::PointLight::PointLight()
{
	std::function<void()>* f = new std::function<void()>([=] {
		SpriteRenderer* sr = m_OwnerOfCom->GetComponent<SpriteRenderer>();
		if (sr == nullptr)
			sr = m_OwnerOfCom->AddComponent<SpriteRenderer>();
		sr->m_DisableRotation = true;
		sr->m_Texture = Texture::Get("src/UIimages/Lamp.png");
		s_PointLights.push_back(this);
		});
	EventSystem::GetInstance().SendEvent(EventType::ONMAINTHREADPROCESS, nullptr, f);
}

Doom::PointLight::~PointLight()
{
	auto iter = std::find(s_PointLights.begin(), s_PointLights.end(), this);
	if (iter != s_PointLights.end()) 
		s_PointLights.erase(iter);
}
