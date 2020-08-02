#include "../pch.h"
#include "DirectionalLight.h"

using namespace Doom;

Doom::DirectionalLight::DirectionalLight()
{
	SetType(ComponentTypes::DIRECTIONALLIGHT);
	dirLights.push_back(this);
}

Doom::DirectionalLight::~DirectionalLight()
{
	auto iter = std::find(dirLights.begin(), dirLights.end(), this);
	if (iter != dirLights.end()) {
		dirLights.erase(iter);
	}
}

