#include "../pch.h"
#include "PointLight.h"

using namespace Doom;

Doom::PointLight::PointLight()
{
	SetType(ComponentType::POINTLIGHT);
	pLights.push_back(this);
}

Doom::PointLight::~PointLight()
{
	auto iter = std::find(pLights.begin(), pLights.end(), this);
	if (iter != pLights.end()) {
		pLights.erase(iter);
	}
}
