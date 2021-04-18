#include "../pch.h"
#include "Render3D.h"
#include "../Core/Editor.h"
#include "../Render/Instancing.h"
#include "../Components/PointLight.h"

void Doom::Renderer3D::ChangeRenderTechnic(RenderTechnic rt)
{
	if (m_IsTransparent) return;
	if (rt == RenderTechnic::Instancing)
	{
		for (auto i = Instancing::GetInstance()->m_InstancedObjects.begin(); i != Instancing::GetInstance()->m_InstancedObjects.end(); i++)
		{
			if (i->first == m_Mesh) 
			{
				auto iterFind = std::find(i->second.begin(), i->second.end(), this);
				if (iterFind == i->second.end())
				{
					i->second.push_back(this);
					auto iter = std::find(Renderer::s_Objects3d.begin(), Renderer::s_Objects3d.end(), this);
					if (iter != Renderer::s_Objects3d.end())
						Renderer::s_Objects3d.erase(iter);
				}
			}
		}
	}
	else if (rt == RenderTechnic::Forward)
	{
		EraseFromInstancing();
		auto iter = std::find(Renderer::s_Objects3d.begin(), Renderer::s_Objects3d.end(), this);
		if (iter == Renderer::s_Objects3d.end())
		{
			m_Shader = Shader::Get("Default3D");
			Renderer::s_Objects3d.push_back(this);
		}
	}
	m_RenderTechnic = rt;
}

void Doom::Renderer3D::LoadMesh(Mesh* mesh)
{
	EraseFromInstancing();
	m_Mesh = mesh;
	ChangeRenderTechnic(m_RenderTechnic);
	if (m_Mesh != nullptr && !m_IsSkyBox)
	{
		CubeCollider3D* cc = m_OwnerOfCom->m_ComponentManager.GetComponent<CubeCollider3D>();
		if (cc == nullptr) 
		{
			cc = m_OwnerOfCom->m_ComponentManager.AddComponent<CubeCollider3D>();
			cc->m_IsBoundingBox = true;
		}
		cc->m_MinP = m_Mesh->m_TheLowestPoint;
		cc->m_MaxP = m_Mesh->m_TheHighestPoint;
		cc->m_Offset = ((cc->m_MaxP - cc->m_MinP) * 0.5f) + cc->m_MinP;
	}
	m_Tr = m_OwnerOfCom->m_ComponentManager.GetComponent<Transform>();
	//std::cout << "offset " << cc->offset.x << " " << cc->offset.y << " " << cc->offset.z << "\n";
	//std::cout << "the highest " << mesh->theHighestPoint.x << " " << mesh->theHighestPoint.y << " " << mesh->theHighestPoint.z << "\n";
	//std::cout << "the lowest " << mesh->theLowestPoint.x << " " << mesh->theLowestPoint.y << " " << mesh->theLowestPoint.z << "\n";
}

void Doom::Renderer3D::operator=(const Renderer3D& rhs)
{
	Copy(rhs);
}

void Doom::Renderer3D::EraseFromInstancing()
{
	if (m_RenderTechnic == RenderTechnic::Instancing)
	{
		for (auto i = Instancing::GetInstance()->m_InstancedObjects.begin(); i != Instancing::GetInstance()->m_InstancedObjects.end(); i++)
		{
			if (m_Mesh != nullptr) 
			{
				auto iter = std::find(i->second.begin(), i->second.end(), this);
				if (iter != i->second.end()) 
				{
					i->second.erase(iter);
					m_IsInitializedInInstancing = false;
					//m_Mesh = nullptr;
				}
			}
		}
	}
}

Doom::Renderer3D::Renderer3D(const Renderer3D& rhs)
{
	Copy(rhs);
}

Doom::Renderer3D::Renderer3D()
{
	m_RenderType = RenderType::TYPE_3D;
	//SetType(ComponentType::RENDER3D);
	m_Shader = Shader::Get("Default3D");
	m_DiffuseTexture = Texture::s_WhiteTexture;
	m_NormalMapTexture = Texture::Get("InvalidTexture");
}

Doom::Renderer3D::~Renderer3D()
{
}

void Doom::Renderer3D::Delete()
{
	s_FreeMemory.push_back(m_MemoryPoolPtr);
	if (GetOwnerOfComponent() == nullptr) return;
	CubeCollider3D* cc = GetOwnerOfComponent()->m_ComponentManager.GetComponent<CubeCollider3D>();
	if (cc != nullptr && cc->m_IsBoundingBox)
		GetOwnerOfComponent()->m_ComponentManager.RemoveComponent(cc); //Fix: could be dangerous if there is more than 1 CubeCollider
	if (m_IsTransparent)
	{
		auto iter = std::find(Renderer::s_Objects3dTransparent.begin(), Renderer::s_Objects3dTransparent.end(), this);
		if (iter != Renderer::s_Objects3dTransparent.end())
			Renderer::s_Objects3dTransparent.erase(iter);
	}
	else
	{
		ChangeRenderTechnic(RenderTechnic::Forward);
		auto iter = std::find(Renderer::s_Objects3d.begin(), Renderer::s_Objects3d.end(), this);
		if (iter != Renderer::s_Objects3d.end())
			Renderer::s_Objects3d.erase(iter);
	}
	this->Copy(Renderer3D());
}

Doom::Component* Doom::Renderer3D::Create()
{
	char* ptr = Utils::PreAllocateMemory<Renderer3D>(s_MemoryPool, s_FreeMemory);
	Renderer3D* component = (Renderer3D*)((void*)ptr);
	component->m_MemoryPoolPtr = ptr;
	if (component->m_IsTransparent) Renderer::s_Objects3dTransparent.push_back(component);
	else Renderer::s_Objects3d.push_back(component);
	component->m_Mesh = nullptr;
	return component;
}

#include "../Core/Timer.h"
#include "DirectionalLight.h"

void Doom::Renderer3D::BakeShadows()
{
	if (m_IsCastingShadows && m_Mesh != nullptr && m_Mesh->m_IsInitialized) 
	{
		if (!m_IsSkyBox) 
		{
			Shader* bakeShader = Shader::Get("BakeShadows");
			bakeShader->Bind();
			glBindTextureUnit(0, m_DiffuseTexture->m_RendererID);
			bakeShader->SetUniformMat4f("u_Model", m_Tr->m_PosMat4);
			bakeShader->SetUniformMat4f("lightSpaceMatrix", DirectionalLight::GetLightSpaceMatrix());
			bakeShader->SetUniformMat4f("u_Scale", m_Tr->m_ScaleMat4);
			bakeShader->SetUniformMat4f("u_View", m_Tr->m_ViewMat4);
			bakeShader->Bind();
			m_Mesh->m_Va.Bind();
			m_Mesh->m_Ib.Bind();
			m_Mesh->m_Vb.Bind();
			Renderer::s_Stats.m_Vertices += m_Mesh->m_IndicesSize;
			Renderer::s_Stats.m_DrawCalls++;
			glDrawElements(GL_TRIANGLES, m_Mesh->m_Ib.m_count, GL_UNSIGNED_INT, nullptr);
			bakeShader->UnBind();
			m_Mesh->m_Ib.UnBind();
			glBindTextureUnit(0, Texture::s_WhiteTexture->m_RendererID);
		}
	}
}

void Doom::Renderer3D::MakeTransparent()
{
	auto iter = std::find(Renderer::s_Objects3d.begin(), Renderer::s_Objects3d.end(), this);
	if(iter != Renderer::s_Objects3d.end()) 
	{
		EraseFromInstancing();
		ChangeRenderTechnic(RenderTechnic::Forward);
		Renderer::s_Objects3d.erase(iter);
		Renderer::s_Objects3dTransparent.push_back(this);
		m_IsTransparent = true;
	}
}

void Doom::Renderer3D::MakeSolid()
{
	auto iter = std::find(Renderer::s_Objects3dTransparent.begin(), Renderer::s_Objects3dTransparent.end(), this);
	if (iter != Renderer::s_Objects3dTransparent.end()) 
	{
		Renderer::s_Objects3dTransparent.erase(iter);
		Renderer::s_Objects3d.push_back(this);
		ChangeRenderTechnic(RenderTechnic::Forward);
		m_IsTransparent = false;
	}
}

void Doom::Renderer3D::Render()
{
	if (m_RenderTechnic == RenderTechnic::Forward) 
	{
		m_Tr = &(GetOwnerOfComponent()->m_Transform);
		if (m_IsWireMesh) 
		{
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			ForwardRender(m_Tr->m_PosMat4, m_Tr->m_ViewMat4, m_Tr->m_ScaleMat4, m_Color);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else 
		{
			ForwardRender(m_Tr->m_PosMat4, m_Tr->m_ViewMat4, m_Tr->m_ScaleMat4, m_Color);
		}
			
	}
}

void Doom::Renderer3D::Copy(const Renderer3D& rhs)
{
	m_FloatUniforms = rhs.m_FloatUniforms;
	m_Material = rhs.m_Material;
	m_DiffuseTexture = rhs.m_DiffuseTexture;
	m_NormalMapTexture = rhs.m_NormalMapTexture;
	LoadMesh(rhs.m_Mesh);
	m_IsCastingShadows = rhs.m_IsCastingShadows;
	m_IsWireMesh = rhs.m_IsWireMesh;
	m_IsUsingNormalMap = rhs.m_IsUsingNormalMap;
	if (rhs.m_IsTransparent)
		MakeTransparent();
	m_IsSkyBox = rhs.m_IsSkyBox;
	ChangeRenderTechnic(rhs.m_RenderTechnic);
	m_Color = rhs.m_Color;
	m_Shader = rhs.m_Shader;
	m_RenderType = rhs.m_RenderType;
}

void Doom::Renderer3D::ForwardRender(glm::mat4& pos, glm::mat4& view, glm::mat4& scale, glm::vec4& color)
{
	if (m_Mesh != nullptr && m_Mesh->m_IsInitialized)
	{
		if (m_IsSkyBox) {
			RenderSkyBox();
			return;
		}
		m_Shader->Bind();
		glBindTextureUnit(0, m_DiffuseTexture->m_RendererID);
		m_Shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().GetViewProjectionMatrix());
		m_Shader->SetUniformMat4f("u_Model", pos);
		m_Shader->SetUniformMat4f("u_View", view);
		m_Shader->SetUniformMat4f("u_Scale", scale);
		m_Shader->SetUniform4f("m_color", color[0], color[1], color[2], color[3]);
		AdditionalUniformsLoad();
		int dlightSize = DirectionalLight::s_DirLights.size();
		m_Shader->SetUniform1i("dLightSize", dlightSize);
		char buffer[64];
		for (int i = 0; i < dlightSize; i++)
		{
			DirectionalLight* dl = DirectionalLight::s_DirLights[i];
			dl->m_Dir = dl->GetOwnerOfComponent()->m_Transform.m_ViewMat4 * glm::vec4(0, 0, -1, 1);
			sprintf(buffer, "dirLights[%i].dir", i);
			m_Shader->SetUniform3fv(buffer, dl->m_Dir);
			sprintf(buffer, "dirLights[%i].color", i);
			m_Shader->SetUniform3fv(buffer, dl->m_Color);
		}

		int plightSize = PointLight::s_PointLights.size();
		m_Shader->SetUniform1i("pLightSize", plightSize);
		for (int i = 0; i < plightSize; i++)
		{
			PointLight* pl = PointLight::s_PointLights[i];
			sprintf(buffer, "pointLights[%i].position", i);
			m_Shader->SetUniform3fv(buffer, pl->GetOwnerOfComponent()->GetPosition());
			sprintf(buffer, "pointLights[%i].color", i);
			m_Shader->SetUniform3fv(buffer, pl->m_Color);
			sprintf(buffer, "pointLights[%i].constant", i);
			m_Shader->SetUniform1f(buffer, pl->m_Constant);
			sprintf(buffer, "pointLights[%i]._linear", i);
			m_Shader->SetUniform1f(buffer, pl->m_Linear);
			sprintf(buffer, "pointLights[%i].quadratic", i);
			m_Shader->SetUniform1f(buffer, pl->m_Quadratic);
		}

		int slightSize = SpotLight::s_SpotLights.size();
		m_Shader->SetUniform1i("sLightSize", slightSize);
		for (int i = 0; i < slightSize; i++)
		{
			SpotLight* sl = SpotLight::s_SpotLights[i];
			sprintf(buffer, "spotLights[%i].position", i);
			m_Shader->SetUniform3fv(buffer, sl->GetOwnerOfComponent()->GetPosition());
			sprintf(buffer, "spotLights[%i].dir", i);
			m_Shader->SetUniform3fv(buffer, sl->GetOwnerOfComponent()->m_Transform.m_ViewMat4 * glm::vec4(0, 0, -1, 1));
			sprintf(buffer, "spotLights[%i].color", i);
			m_Shader->SetUniform3fv(buffer, sl->m_Color);
			sprintf(buffer, "spotLights[%i].constant", i);
			m_Shader->SetUniform1f(buffer, sl->m_Constant);
			sprintf(buffer, "spotLights[%i]._linear", i);
			m_Shader->SetUniform1f(buffer, sl->m_Linear);
			sprintf(buffer, "spotLights[%i].quadratic", i);
			m_Shader->SetUniform1f(buffer, sl->m_Quadratic);
			sprintf(buffer, "spotLights[%i].innerCutOff", i);
			m_Shader->SetUniform1f(buffer, sl->m_InnerCutOff);
			sprintf(buffer, "spotLights[%i].outerCutOff", i);
			m_Shader->SetUniform1f(buffer, sl->m_OuterCutOff);
		}

		m_Shader->SetUniform3fv("u_CameraPos", Window::GetInstance().GetCamera().GetPosition());
		m_Shader->SetUniform1f("u_Ambient", m_Material.m_Ambient);
		m_Shader->SetUniform1f("u_Specular", m_Material.m_Specular);
		m_Shader->SetUniformMat4f("u_lightSpaceMatrix", DirectionalLight::GetLightSpaceMatrix());
		m_Shader->SetUniform1i("u_DiffuseTexture", 0);
		glBindTextureUnit(2, Window::GetInstance().m_FrameBufferShadowMap->m_Textures[0]);
		m_Shader->SetUniform1i("u_ShadowTexture", 2);
		m_Shader->SetUniform1f("u_DrawShadows", Instancing::GetInstance()->m_DrawShadows); 
		m_Shader->SetUniform1f("Brightness", Renderer::s_Brightness);
		if (m_IsUsingNormalMap)
		{
			glBindTextureUnit(1, m_NormalMapTexture->m_RendererID);
			m_Shader->SetUniform1i("u_NormalMapTexture", 1);
		}
		m_Shader->SetUniform1i("u_isNormalMapping", m_IsUsingNormalMap);
		m_Mesh->m_Va.Bind();
		m_Mesh->m_Ib.Bind();

		Renderer::s_Stats.m_Vertices += m_Mesh->m_Ib.m_count;
		Renderer::s_Stats.m_DrawCalls++;
		if (!m_IsCullingFace)
			glDisable(GL_CULL_FACE);

		if (m_HighLight)
		{
			//Need to refactor
			glEnable(GL_STENCIL_TEST);

			glStencilFunc(GL_ALWAYS, 1, 0xFF);
			glStencilMask(0xFF);
			m_Shader->Bind();
			glDrawElements(GL_TRIANGLES, m_Mesh->m_Ib.m_count, GL_UNSIGNED_INT, nullptr);

			Renderer::s_OutLined3dObjects.push_back(this);

			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			glStencilMask(0x00);
			Shader* shader = Shader::Get("HighLightBorder");
			shader->Bind();
			shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().GetViewProjectionMatrix());
			shader->SetUniformMat4f("u_Model", pos);
			shader->SetUniformMat4f("u_View", view);
			glm::vec3 camPos = Window::GetInstance().GetCamera().GetPosition();
			glm::vec3 d = camPos - GetOwnerOfComponent()->GetPosition();
			glm::vec3 scaleV3 = GetOwnerOfComponent()->GetScale();
			float distance = glm::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
			glm::vec3 koef = glm::vec3(1.0f) + glm::vec3(distance / scaleV3.x, distance / scaleV3.y, distance / scaleV3.z) * 0.003f;
			koef = glm::clamp(koef, 1.00f, 100000.0f);
			shader->SetUniformMat4f("u_Scale", glm::scale(scale, koef));
			glDrawElements(GL_TRIANGLES, m_Mesh->m_Ib.m_count, GL_UNSIGNED_INT, nullptr);

			glStencilMask(0xFF);
			glDisable(GL_STENCIL_TEST);
		}
		else
			glDrawElements(GL_TRIANGLES, m_Mesh->m_Ib.m_count, GL_UNSIGNED_INT, nullptr);
		glEnable(GL_CULL_FACE);
		m_Shader->UnBind();
		m_Mesh->m_Ib.UnBind();
		glBindTextureUnit(0, Texture::s_WhiteTexture->m_RendererID);
	}
}

void Doom::Renderer3D::RenderSkyBox()
{
	m_Shader = Shader::Get("SkyBox");
	glDepthFunc(GL_LEQUAL);
	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_CULL_FACE);
	glBindTexture(GL_TEXTURE_CUBE_MAP, m_DiffuseTexture->m_RendererID);
	m_Shader->Bind();
	Window::GetInstance().GetCamera().RecalculateViewMatrix();
	m_Shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().GetProjectionMatrix());
	m_Shader->SetUniformMat4f("u_Model", m_Tr->m_PosMat4);
	m_Shader->SetUniformMat4f("u_View", glm::mat4(glm::mat3(Window::GetInstance().GetCamera().GetViewMatrix())));
	m_Shader->SetUniformMat4f("u_Scale", m_Tr->m_ScaleMat4);
	m_Shader->SetUniform1i("u_DiffuseTexture", 0);
	m_Shader->SetUniform1f("Brightness", Renderer::s_Brightness);
	m_Shader->Bind();
	m_Mesh->m_Va.Bind();
	m_Mesh->m_Ib.Bind();
	m_Mesh->m_Vb.Bind();
	Renderer::s_Stats.m_Vertices += m_Mesh->m_Ib.m_count;
	Renderer::s_Stats.m_DrawCalls++;
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
	m_Shader->UnBind();
	m_Mesh->m_Ib.UnBind();
	m_DiffuseTexture->UnBind();
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

void Doom::Renderer3D::AdditionalUniformsLoad()
{
	for (auto i = m_FloatUniforms.begin(); i != m_FloatUniforms.end(); i++)
	{
		m_Shader->SetUniform1f(i->first, i->second);
	}
}

