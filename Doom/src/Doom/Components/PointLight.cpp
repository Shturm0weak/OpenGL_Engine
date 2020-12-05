#include "../pch.h"
#include "PointLight.h"

using namespace Doom;

Doom::PointLight::PointLight()
{
	//SetType(ComponentType::POINTLIGHT);
	s_PointLights.push_back(this);
}

Doom::PointLight::~PointLight()
{
	auto iter = std::find(s_PointLights.begin(), s_PointLights.end(), this);
	if (iter != s_PointLights.end()) {
		s_PointLights.erase(iter);
	}
}
