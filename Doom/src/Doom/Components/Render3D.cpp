#include "../pch.h"
#include "Render3D.h"
#include "../Core/Editor.h"
#include "../Render/Instancing.h"
#include "../Components/PointLight.h"

void Doom::Renderer3D::ChangeRenderTechnic(RenderTechnic rt)
{
	renderTechnic = rt;
	if (renderTechnic == RenderTechnic::Instancing) {
		for (auto i = Instancing::Instance()->instancedObjects.begin(); i != Instancing::Instance()->instancedObjects.end(); i++)
		{
			if (i->first == mesh) {
				i->second.push_back(this);
				auto iter = std::find(Renderer::objects3d.begin(), Renderer::objects3d.end(), this);
				if (iter != Renderer::objects3d.end())
					Renderer::objects3d.erase(iter);
			}
		}
	}
}

void Doom::Renderer3D::LoadMesh(Mesh * _mesh)
{
	EraseFromInstancing();
	mesh = _mesh;
	ChangeRenderTechnic(renderTechnic);
}

void Doom::Renderer3D::EraseFromInstancing()
{
	if (renderTechnic == RenderTechnic::Instancing) {
		for (auto i = Instancing::Instance()->instancedObjects.begin(); i != Instancing::Instance()->instancedObjects.end(); i++)
		{
			if (mesh != nullptr) {
				auto iter = std::find(i->second.begin(), i->second.end(), this);
				if (iter != i->second.end()) {
					i->second.erase(iter);
					mesh = nullptr;
				}
			}
		}
	}
}

Doom::Renderer3D::Renderer3D(GameObject* _owner)
{
	renderType = "3D";
	SetType(ComponentTypes::RENDER);
	owner = _owner;
	shader = Shader::Get("Default3D");
	tr = owner->GetComponentManager()->GetComponent<Transform>();
	pos = translate(glm::mat4(1.f), glm::vec3(tr->position.x, tr->position.y, tr->position.z));
	Renderer::objects3d.push_back(this);
}

Doom::Renderer3D::~Renderer3D()
{
}

#include "../Core/Timer.h"
#include "DirectionalLight.h"

void Doom::Renderer3D::BakeShadows()
{
	if (mesh != nullptr) {
		if (!isSkyBox) {
			Shader* bakeShader = Shader::Get("BakeShadows");
			bakeShader->Bind();
			glBindTextureUnit(0, diffuseTexture->m_RendererID);
			bakeShader->SetUniformMat4f("u_Model", pos);
			bakeShader->SetUniformMat4f("lightSpaceMatrix", DirectionalLight::dirLights[0]->lightSpaceMatrix);
			bakeShader->SetUniformMat4f("u_Scale", scale);
			bakeShader->SetUniformMat4f("u_View", view);
			bakeShader->Bind();
			mesh->va->Bind();
			mesh->ib->Bind();
			mesh->vb->Bind();
			Renderer::Vertices += mesh->meshSize * 0.5f;
			Renderer::DrawCalls++;
			glDrawElements(GL_TRIANGLES, mesh->ib->GetCount(), GL_UNSIGNED_INT, nullptr);
			bakeShader->UnBind();
			mesh->ib->UnBind();
		}
	}
}

void Doom::Renderer3D::Render()
{
	if (renderTechnic == RenderTechnic::Forward)
		ForwardRender();
}

void Doom::Renderer3D::ForwardRender()
{
	if (mesh != nullptr) {
		if (!isSkyBox) {
			shader->Bind();
			glBindTextureUnit(0, diffuseTexture->m_RendererID);
			shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
			shader->SetUniformMat4f("u_Model", pos);
			shader->SetUniformMat4f("u_View", view);
			shader->SetUniformMat4f("u_Scale", scale);
			shader->SetUniform4f("m_color", color[0], color[1], color[2], color[3]);

			int dlightSize = DirectionalLight::dirLights.size();
			shader->SetUniform1i("dLightSize", dlightSize);
			for (int i = 0; i < dlightSize; i++)
			{
				char buffer[64];
				DirectionalLight* dl = DirectionalLight::dirLights[i];
				dl->dir = dl->GetOwnerOfComponent()->GetComponent<Irenderer>()->view * glm::vec4(0, 0, -1, 1);
				sprintf(buffer, "dirLights[%i].dir", i);
				shader->SetUniform3fv(buffer, dl->dir);
				sprintf(buffer, "dirLights[%i].color", i);
				shader->SetUniform3fv(buffer, dl->color);
			}

			int plightSize = PointLight::pLights.size();
			shader->SetUniform1i("pLightSize", plightSize);
			char buffer[64];
			for (int i = 0; i < plightSize; i++)
			{
				PointLight* pl = PointLight::pLights[i];
				sprintf(buffer, "pointLights[%i].position", i);
				shader->SetUniform3fv(buffer, pl->GetOwnerOfComponent()->position);
				sprintf(buffer, "pointLights[%i].color", i);
				shader->SetUniform3fv(buffer, pl->color);
				sprintf(buffer, "pointLights[%i].constant", i);
				shader->SetUniform1f(buffer, pl->constant);
				sprintf(buffer, "pointLights[%i]._linear", i);
				shader->SetUniform1f(buffer, pl->linear);
				sprintf(buffer, "pointLights[%i].quadratic", i);
				shader->SetUniform1f(buffer, pl->quadratic);
			}
			shader->SetUniform3fv("u_CameraPos", Window::GetCamera().GetPosition());
			shader->SetUniform1f("u_Ambient", mat.ambient);
			shader->SetUniform1f("u_Specular", mat.specular);
			shader->SetUniformMat4f("u_lightSpaceMatrix", DirectionalLight::dirLights[0]->lightSpaceMatrix);
			shader->SetUniform1i("u_DiffuseTexture", 0);
			glBindTextureUnit(2, Window::GetCamera().frameBufferShadowMap->texture);
			shader->SetUniform1i("u_ShadowTexture", 2);
			shader->SetUniform1f("u_DrawShadows", Instancing::Instance()->drawShadows);
			if (useNormalMap) {
				glBindTextureUnit(1, normalMapTexture->m_RendererID);
				shader->SetUniform1i("u_NormalMapTexture", 1);
			}
			shader->SetUniform1i("u_isNormalMapping", useNormalMap);
			shader->Bind();
			mesh->va->Bind();
			mesh->ib->Bind();
			mesh->vb->Bind();
			Renderer::Vertices += mesh->meshSize * 0.5f;
			Renderer::DrawCalls++;
			glDrawElements(GL_TRIANGLES, mesh->ib->GetCount(), GL_UNSIGNED_INT, nullptr);
			shader->UnBind();
			mesh->ib->UnBind();
			glBindTextureUnit(0, 0);
		}
		else {
			//shader = Shader::Get("SkyBoxShader");
			glDepthFunc(GL_LEQUAL);
			glActiveTexture(GL_TEXTURE0);
			glDisable(GL_CULL_FACE);
			glBindTexture(GL_TEXTURE_CUBE_MAP, diffuseTexture->m_RendererID);
			shader->Bind();
			Window::GetCamera().RecalculateViewMatrix();
			shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetProjectionMatrix());
			shader->SetUniformMat4f("u_Model", pos);
			shader->SetUniformMat4f("u_View", glm::mat4(glm::mat3(Window::GetCamera().GetViewMatrix())));
			shader->SetUniformMat4f("u_Scale", scale);
			shader->SetUniform1i("u_DiffuseTexture", 0);
			shader->Bind();
			mesh->va->Bind();
			mesh->ib->Bind();
			mesh->vb->Bind();
			Renderer::DrawCalls++;
			glDrawArrays(GL_TRIANGLES, 0, 36);
			glBindVertexArray(0);
			shader->UnBind();
			mesh->ib->UnBind();
			diffuseTexture->UnBind();
			glDepthFunc(GL_LESS);
			glEnable(GL_CULL_FACE);
		}
	}
}

