#include "../pch.h"
#include "Render3D.h"
#include "../Core/Editor.h"
#include "../Render/Instancing.h"
#include "../Components/PointLight.h"

void Doom::Renderer3D::ChangeRenderTechnic(RenderTechnic rt)
{
	if (m_IsTransparent)
		return;
	m_RenderTechnic = rt;
	if (m_RenderTechnic == RenderTechnic::Instancing) {
		for (auto i = Instancing::Instance()->instancedObjects.begin(); i != Instancing::Instance()->instancedObjects.end(); i++)
		{
			if (i->first == m_Mesh) {
				i->second.push_back(this);
				auto iter = std::find(Renderer::objects3d.begin(), Renderer::objects3d.end(), this);
				if (iter != Renderer::objects3d.end())
					Renderer::objects3d.erase(iter);
			}
		}
	}
	else if (m_RenderTechnic == RenderTechnic::Forward) {
		auto iter = std::find(Renderer::objects3d.begin(), Renderer::objects3d.end(), this);
		if (iter == Renderer::objects3d.end()) {
			m_Shader = Shader::Get("Default3D");
			Renderer::objects3d.push_back(this);
		}
	}
}

void Doom::Renderer3D::LoadMesh(Mesh * _mesh)
{
	m_Tr = m_Owner->GetComponentManager()->GetComponent<Transform>();
	EraseFromInstancing();
	m_Mesh = _mesh;
	ChangeRenderTechnic(RenderTechnic::Forward);
	if (!m_IsSkyBox) {
		CubeCollider3D* cc = m_Owner->GetComponentManager()->GetComponent<CubeCollider3D>();
		if (cc == nullptr) {
			cc = m_Owner->GetComponentManager()->AddComponent<CubeCollider3D>();
			cc->m_IsBoundingBox = true;
		}
		cc->m_MinP = m_Mesh->theLowestPoint;
		cc->m_MaxP = m_Mesh->theHighestPoint;
		cc->m_Offset = (cc->m_MaxP - (glm::abs(cc->m_MinP) + glm::abs(cc->m_MaxP)) * 0.5f);
	}
	//std::cout << "offset " << cc->offset.x << " " << cc->offset.y << " " << cc->offset.z << "\n";
	//std::cout << "the highest " << mesh->theHighestPoint.x << " " << mesh->theHighestPoint.y << " " << mesh->theHighestPoint.z << "\n";
	//std::cout << "the lowest " << mesh->theLowestPoint.x << " " << mesh->theLowestPoint.y << " " << mesh->theLowestPoint.z << "\n";
}

void Doom::Renderer3D::EraseFromInstancing()
{
	if (m_RenderTechnic == RenderTechnic::Instancing) {
		for (auto i = Instancing::Instance()->instancedObjects.begin(); i != Instancing::Instance()->instancedObjects.end(); i++)
		{
			if (m_Mesh != nullptr) {
				auto iter = std::find(i->second.begin(), i->second.end(), this);
				if (iter != i->second.end()) {
					i->second.erase(iter);
					m_Mesh = nullptr;
				}
			}
		}
	}
}

Doom::Renderer3D::Renderer3D()
{
	m_RenderType = RenderType::TYPE_3D;
	//SetType(ComponentType::RENDER3D);
	m_Shader = Shader::Get("Default3D");
	if(m_IsTransparent)
		Renderer::objects3dTransparent.push_back(this);
	else
		Renderer::objects3d.push_back(this);
}

Doom::Renderer3D::~Renderer3D()
{
	CubeCollider3D* cc = GetOwnerOfComponent()->GetComponentManager()->GetComponent<CubeCollider3D>();
	if (cc != nullptr && cc->m_IsBoundingBox) {
		GetOwnerOfComponent()->GetComponentManager()->RemoveComponent(cc);
	}
	if (m_IsTransparent) {
		auto iter = std::find(Renderer::objects3dTransparent.begin(), Renderer::objects3dTransparent.end(), this);
		if (iter != Renderer::objects3dTransparent.end()) {
			Renderer::objects3dTransparent.erase(iter);
		}
	}
	else {
		auto iter = std::find(Renderer::objects3d.begin(), Renderer::objects3d.end(), this);
		if (iter != Renderer::objects3d.end()) {
			Renderer::objects3d.erase(iter);
		}
	}
}

#include "../Core/Timer.h"
#include "DirectionalLight.h"

void Doom::Renderer3D::BakeShadows()
{
	if (m_IsCastingShadows && m_Mesh != nullptr) {
		if (!m_IsSkyBox) {
			Shader* bakeShader = Shader::Get("BakeShadows");
			bakeShader->Bind();
			glBindTextureUnit(0, m_DiffuseTexture->m_RendererID);
			bakeShader->SetUniformMat4f("u_Model", m_Tr->m_PosMat4);
			bakeShader->SetUniformMat4f("lightSpaceMatrix", DirectionalLight::GetLightSpaceMatrix());
			bakeShader->SetUniformMat4f("u_Scale", m_Tr->m_ScaleMat4);
			bakeShader->SetUniformMat4f("u_View", m_Tr->m_ViewMat4);
			bakeShader->Bind();
			m_Mesh->va->Bind();
			m_Mesh->ib->Bind();
			m_Mesh->vb->Bind();
			Renderer::Vertices += m_Mesh->indicesSize;
			Renderer::DrawCalls++;
			glDrawElements(GL_TRIANGLES, m_Mesh->ib->GetCount(), GL_UNSIGNED_INT, nullptr);
			bakeShader->UnBind();
			m_Mesh->ib->UnBind();
			glBindTextureUnit(0, Texture::WhiteTexture->m_RendererID);
		}
	}
}

void Doom::Renderer3D::MakeTransparent()
{
	auto iter = std::find(Renderer::objects3d.begin(), Renderer::objects3d.end(), this);
	if(iter != Renderer::objects3d.end()) {
		EraseFromInstancing();
		ChangeRenderTechnic(RenderTechnic::Forward);
		Renderer::objects3d.erase(iter);
		Renderer::objects3dTransparent.push_back(this);
		m_IsTransparent = true;
	}
}

void Doom::Renderer3D::MakeSolid()
{
	auto iter = std::find(Renderer::objects3dTransparent.begin(), Renderer::objects3dTransparent.end(), this);
	if (iter != Renderer::objects3dTransparent.end()) {
		Renderer::objects3dTransparent.erase(iter);
		Renderer::objects3d.push_back(this);
		ChangeRenderTechnic(RenderTechnic::Forward);
		m_IsTransparent = false;
	}
}

void Doom::Renderer3D::Render()
{
	if (m_RenderTechnic == RenderTechnic::Forward) {
		if (m_IsWireMesh) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			ForwardRender(m_Tr->m_PosMat4, m_Tr->m_ViewMat4, m_Tr->m_ScaleMat4, m_Color);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else {
			ForwardRender(m_Tr->m_PosMat4, m_Tr->m_ViewMat4, m_Tr->m_ScaleMat4, m_Color);
		}
			
	}
}

void Doom::Renderer3D::ForwardRender(glm::mat4& pos, glm::mat4& view, glm::mat4& scale, glm::vec4& color)
{
	if (m_Mesh != nullptr) {
		if (!m_IsSkyBox) {
			m_Shader->Bind();
			glBindTextureUnit(0, m_DiffuseTexture->m_RendererID);
			m_Shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
			m_Shader->SetUniformMat4f("u_Model", pos);
			m_Shader->SetUniformMat4f("u_View", view);
			m_Shader->SetUniformMat4f("u_Scale", scale);
			m_Shader->SetUniform4f("m_color", color[0], color[1], color[2], color[3]);
			AdditionalUniformsLoad();
			int dlightSize = DirectionalLight::dirLights.size();
			m_Shader->SetUniform1i("dLightSize", dlightSize);
			for (int i = 0; i < dlightSize; i++)
			{
				char buffer[64];
				DirectionalLight* dl = DirectionalLight::dirLights[i];
				dl->m_Dir = dl->GetOwnerOfComponent()->GetComponent<Transform>()->m_ViewMat4 * glm::vec4(0, 0, -1, 1);
				sprintf(buffer, "dirLights[%i].dir", i);
				m_Shader->SetUniform3fv(buffer, dl->m_Dir);
				sprintf(buffer, "dirLights[%i].color", i);
				m_Shader->SetUniform3fv(buffer, dl->m_Color);
			}

			int plightSize = PointLight::pLights.size();
			m_Shader->SetUniform1i("pLightSize", plightSize);
			char buffer[64];
			for (int i = 0; i < plightSize; i++)
			{
				PointLight* pl = PointLight::pLights[i];
				sprintf(buffer, "pointLights[%i].position", i);
				m_Shader->SetUniform3fv(buffer, pl->GetOwnerOfComponent()->GetPosition());
				sprintf(buffer, "pointLights[%i].color", i);
				m_Shader->SetUniform3fv(buffer, pl->color);
				sprintf(buffer, "pointLights[%i].constant", i);
				m_Shader->SetUniform1f(buffer, pl->m_Constant);
				sprintf(buffer, "pointLights[%i]._linear", i);
				m_Shader->SetUniform1f(buffer, pl->m_Linear);
				sprintf(buffer, "pointLights[%i].quadratic", i);
				m_Shader->SetUniform1f(buffer, pl->m_Quadratic);
			}
			m_Shader->SetUniform3fv("u_CameraPos", Window::GetCamera().GetPosition());
			m_Shader->SetUniform1f("u_Ambient", m_Material.m_Ambient);
			m_Shader->SetUniform1f("u_Specular", m_Material.m_Specular);
			m_Shader->SetUniformMat4f("u_lightSpaceMatrix", DirectionalLight::GetLightSpaceMatrix());
			m_Shader->SetUniform1i("u_DiffuseTexture", 0);
			glBindTextureUnit(2, Window::GetCamera().m_FrameBufferShadowMap->texture);
			m_Shader->SetUniform1i("u_ShadowTexture", 2);
			m_Shader->SetUniform1f("u_DrawShadows", Instancing::Instance()->drawShadows);
			if (m_IsUsingNormalMap) {
				glBindTextureUnit(1, m_NormalMapTexture->m_RendererID);
				m_Shader->SetUniform1i("u_NormalMapTexture", 1);
			}
			m_Shader->SetUniform1i("u_isNormalMapping", m_IsUsingNormalMap);
			m_Shader->Bind();
			m_Mesh->va->Bind();
			m_Mesh->ib->Bind();
			m_Mesh->vb->Bind();
			Renderer::Vertices += m_Mesh->ib->GetCount();
			Renderer::DrawCalls++;
			glDrawElements(GL_TRIANGLES, m_Mesh->ib->GetCount(), GL_UNSIGNED_INT, nullptr);
			m_Shader->UnBind();
			m_Mesh->ib->UnBind();
			glBindTextureUnit(0, Texture::WhiteTexture->m_RendererID);
		}
		else {
			//shader = Shader::Get("SkyBoxShader");
			glDepthFunc(GL_LEQUAL);
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_CULL_FACE);
			glBindTexture(GL_TEXTURE_CUBE_MAP, m_DiffuseTexture->m_RendererID);
			m_Shader->Bind();
			Window::GetCamera().RecalculateViewMatrix();
			m_Shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetProjectionMatrix());
			m_Shader->SetUniformMat4f("u_Model", m_Tr->m_PosMat4);
			m_Shader->SetUniformMat4f("u_View", glm::mat4(glm::mat3(Window::GetCamera().GetViewMatrix())));
			m_Shader->SetUniformMat4f("u_Scale", m_Tr->m_ScaleMat4);
			m_Shader->SetUniform1i("u_DiffuseTexture", 0);
			m_Shader->Bind();
			m_Mesh->va->Bind();
			m_Mesh->ib->Bind();
			m_Mesh->vb->Bind();
			Renderer::Vertices += m_Mesh->ib->GetCount();
			Renderer::DrawCalls++;
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			m_Shader->UnBind();
			m_Mesh->ib->UnBind();
			m_DiffuseTexture->UnBind();
			glDepthFunc(GL_LESS);
			glEnable(GL_CULL_FACE);
		}
	}
}

void Doom::Renderer3D::AdditionalUniformsLoad()
{
	for (auto i = m_FloatUniforms.begin(); i != m_FloatUniforms.end(); i++)
	{
		m_Shader->SetUniform1f(i->first, i->second);
	}
}

