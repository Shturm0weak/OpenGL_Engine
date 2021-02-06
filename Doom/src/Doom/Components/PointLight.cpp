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
	//SetType(ComponentType::POINTLIGHT);
	s_PointLights.push_back(this);
}

Doom::PointLight::~PointLight()
{
	auto iter = std::find(s_PointLights.begin(), s_PointLights.end(), this);
	if (iter != s_PointLights.end()) 
		s_PointLights.erase(iter);
}
