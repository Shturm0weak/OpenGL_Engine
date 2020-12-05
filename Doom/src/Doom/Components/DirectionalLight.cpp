#include "../pch.h"
#include "DirectionalLight.h"

using namespace Doom;

glm::mat4 Doom::DirectionalLight::GetLightSpaceMatrix()
{
	if (s_DirLights.size() > 0)
		return s_DirLights[0]->lightSpaceMatrix;
	else
		return glm::mat4(1.0f);
}

void Doom::DirectionalLight::UpdateLightMatrix()
{
	Camera& cam = Window::GetCamera();
	float near_plane = cam.m_ZnearSM, far_plane = cam.m_ZfarSM;
	m_LightProjection = glm::ortho(-cam.m_RationProjectionSM, cam.m_RationProjectionSM, -cam.m_RationProjectionSM, cam.m_RationProjectionSM, near_plane, far_plane);
	glm::vec3 dir = GetOwnerOfComponent()->GetComponent<Transform>()->m_ViewMat4 * glm::vec4(0, 0, 1, 1);
	glm::vec3 pos = Window::GetCamera().GetPosition();
	glm::mat4 view = glm::lookAt(pos, dir + pos, glm::vec3(0, 1, 0));
	lightSpaceMatrix = m_LightProjection * view;
}

Doom::DirectionalLight::DirectionalLight()
{
	//SetType(ComponentType::DIRECTIONALLIGHT);
	s_DirLights.push_back(this);
}

Doom::DirectionalLight::~DirectionalLight()
{
	auto iter = std::find(s_DirLights.begin(), s_DirLights.end(), this);
	if (iter != s_DirLights.end()) {
		s_DirLights.erase(iter);
	}
}