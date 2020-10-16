#include "../pch.h"
#include "DirectionalLight.h"

using namespace Doom;

void Doom::DirectionalLight::UpdateLightMatrix()
{
	Camera& cam = Window::GetCamera();
	float near_plane = cam.znears, far_plane = cam.zfars;
	lightProjection = glm::ortho(-cam.rationprojections, cam.rationprojections, -cam.rationprojections, cam.rationprojections, near_plane, far_plane);
	glm::vec3 dir = GetOwnerOfComponent()->GetComponent<Irenderer>()->view * glm::vec4(0, 0, 1, 1);
	glm::vec3 pos = GetOwnerOfComponent()->GetPositions();
	glm::mat4 view = glm::lookAt(pos, dir + pos, glm::vec3(0, 1, 0));
	lightSpaceMatrix = lightProjection * view;
}

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

