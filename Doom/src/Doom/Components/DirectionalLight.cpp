#include "../pch.h"
#include "DirectionalLight.h"

using namespace Doom;

glm::mat4 Doom::DirectionalLight::GetLightSpaceMatrix()
{
	if (dirLights.size() > 0)
		return dirLights[0]->lightSpaceMatrix;
	else
		return glm::mat4(1.0f);
}

void Doom::DirectionalLight::UpdateLightMatrix()
{
	Camera& cam = Window::GetCamera();
	float near_plane = cam.znears, far_plane = cam.zfars;
	lightProjection = glm::ortho(-cam.rationprojections, cam.rationprojections, -cam.rationprojections, cam.rationprojections, near_plane, far_plane);
	glm::vec3 dir = GetOwnerOfComponent()->GetComponent<Transform>()->view * glm::vec4(0, 0, 1, 1);
	glm::vec3 pos = Window::GetCamera().GetPosition();
	glm::mat4 view = glm::lookAt(pos, dir + pos, glm::vec3(0, 1, 0));
	lightSpaceMatrix = lightProjection * view;
}

Doom::DirectionalLight::DirectionalLight()
{
	//SetType(ComponentType::DIRECTIONALLIGHT);
	dirLights.push_back(this);
}

Doom::DirectionalLight::~DirectionalLight()
{
	auto iter = std::find(dirLights.begin(), dirLights.end(), this);
	if (iter != dirLights.end()) {
		dirLights.erase(iter);
	}
}