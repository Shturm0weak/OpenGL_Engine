#include "../pch.h"
#include "DirectionalLight.h"

using namespace Doom;

void Doom::DirectionalLight::Copy(const DirectionalLight& rhs)
{
	m_Intensity = rhs.m_Intensity;
	m_Color = rhs.m_Color;
}

glm::mat4 Doom::DirectionalLight::GetLightSpaceMatrix()
{
	if (s_DirLights.size() > 0) return s_DirLights[0]->lightSpaceMatrix;
	else return glm::mat4(1.0f);
}

void Doom::DirectionalLight::UpdateLightMatrix()
{
	Renderer::ShadowMap& shadowMap = Renderer::s_ShadowMap;
	float nearPlane = shadowMap.m_Znear, far_plane = shadowMap.m_Zfar;
	m_LightProjection = glm::ortho(-shadowMap.m_Zoom, shadowMap.m_Zoom, -shadowMap.m_Zoom, shadowMap.m_Zoom, nearPlane, far_plane);
	glm::vec3 dir = m_OwnerOfCom->m_Transform.m_ViewMat4 * glm::vec4(0, 0, 1, 1);
	glm::vec3 pos = Window::GetInstance().GetCamera().GetPosition();
	glm::mat4 view = glm::lookAt(pos, dir + pos, glm::vec3(0, 1, 0));
	lightSpaceMatrix = m_LightProjection * view;
}

void Doom::DirectionalLight::operator=(const DirectionalLight& rhs)
{
	Copy(rhs);
}

Component* Doom::DirectionalLight::Create()
{
	return new DirectionalLight();
}

Doom::DirectionalLight::DirectionalLight(const DirectionalLight& rhs)
{
	Copy(rhs);
}

Doom::DirectionalLight::DirectionalLight()
{
	//SetType(ComponentType::DIRECTIONALLIGHT);
	s_DirLights.push_back(this);
}

Doom::DirectionalLight::~DirectionalLight()
{
	auto iter = std::find(s_DirLights.begin(), s_DirLights.end(), this);
	if (iter != s_DirLights.end()) 
		s_DirLights.erase(iter);
}