#include "../ECS/ComponentManager.h"
#include "Renderer.h"
#include <iostream>
#include "Batch.h"
#include "../Core/Timer.h"
#include "../Objects/Line.h"
#include "../Core/ViewPort.h"
#include "../Objects/ParticleSystem.h"
#include "../Core/Editor.h"
#include "Instancing.h"
#include "../Core/World.h"

using namespace Doom;

void Renderer::Clear()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearDepth(1.0f);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
}

void Renderer::SortTransparentObjects()
{
	std::sort(s_Objects3dTransparent.begin(), s_Objects3dTransparent.end(), [](Renderer3D* r1, Renderer3D* r2) {
		GameObject* go0 = r1->m_OwnerOfCom;
		GameObject* go1 = r2->m_OwnerOfCom;
		CubeCollider3D* cc0 = nullptr;
		CubeCollider3D* cc1 = nullptr;
		if (r1->m_Mesh != nullptr)
			cc0 = go0->m_ComponentManager.GetComponent<CubeCollider3D>();
		else return false;
		if (r2->m_Mesh != nullptr)
			cc1 = go1->m_ComponentManager.GetComponent<CubeCollider3D>();
		else return false;
		glm::vec3 camPos = Window::GetInstance().GetCamera().GetPosition();
		glm::vec3 pos0 = go0->GetPosition();
		glm::vec3 pos1 = go1->GetPosition();
		float d1 = glm::distance(camPos, pos0 + cc0->m_MinP);
		float d2 = glm::distance(camPos, pos0 + cc0->m_MaxP);
		float r1d = 0;
		float r2d = 0;
		if (d1 < d2) r1d = d1;
		else r1d = d2;
		d1 = glm::distance(camPos, pos1 + cc1->m_MinP);
		d2 = glm::distance(camPos, pos1 + cc1->m_MaxP);
		if (d1 < d2) r2d = d1;
		else r2d = d2;
		return r1d > r2d;
	});
}

void Renderer::RenderBloomEffect()
{
	Window& window = Window::GetInstance();
	if (!s_BloomEffect) return;

	bool horizontal = true, firstIteration = true;
	unsigned int amount = 10;

	std::vector<FrameBuffer*> fb = window.m_FrameBufferBlur;

	Shader* shader = Shader::Get("Blur");
	for (unsigned int i = 0; i < amount; i++)
	{
		fb[horizontal]->Bind();

		shader->Bind();
		shader->SetUniform1i("horizontal", horizontal);
		int id = firstIteration ? window.m_FrameBufferColor->m_Textures[1] : fb[!horizontal]->m_Textures[0];
		glBindTexture(GL_TEXTURE_2D, id);

		Renderer::RenderForPostEffect(Mesh::GetMesh("plane"), shader);
		
		horizontal = !horizontal;
		if (firstIteration) firstIteration = false;
	}
	shader->UnBind();
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Renderer::Clear();

	window.m_FrameBufferColor->Bind();
	shader = Shader::Get("Bloom");
	shader->Bind();
	glBindTextureUnit(0, window.m_FrameBufferColor->m_Textures[0]);
	shader->SetUniform1i("scene", 0);
	glBindTextureUnit(1, fb[0]->m_Textures[0]);
	shader->SetUniform1i("bloomBlurH", 1);
	shader->SetUniform1f("exposure", Renderer::s_Exposure);
	Renderer::RenderForPostEffect(Mesh::GetMesh("plane"), shader);
	window.m_FrameBufferColor->UnBind();

	//ImGui::Begin("Blur");
	//void* te = reinterpret_cast<void*>(fb[!horizontal]->m_Textures[0]);
	//ImGui::Image(te, ImVec2(Window::GetSize()[0], Window::GetSize()[1]), ImVec2(0, 1), ImVec2(1, 0));
	//ImGui::End();
}

void Renderer::RenderOutLined3dObjects()
{
	//@deprecated
	//Right now working version is located at Renderer3D
	glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
	glDisable(GL_DEPTH_TEST);
	for (size_t i = 0; i < s_OutLined3dObjects.size(); i++)
	{
		Renderer3D* r3d = s_OutLined3dObjects[i];
		GameObject* owner = r3d->m_OwnerOfCom;
		
		Shader* shader = Shader::Get("HighLightBorder");
		shader->Bind();
		shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().GetViewProjectionMatrix());
		shader->SetUniformMat4f("u_Model", owner->m_Transform.m_PosMat4);
		shader->SetUniformMat4f("u_View", owner->m_Transform.m_ViewMat4);
		glm::vec3 camPos = Window::GetInstance().GetCamera().GetPosition();
		glm::vec3 d = camPos - owner->GetPosition();
		glm::vec3 scaleV3 = owner->GetScale();
		float distance = glm::sqrt(d.x * d.x + d.y * d.y + d.z * d.z);
		glm::vec3 koef = glm::vec3(1.0f) + glm::vec3(distance / scaleV3.x, distance / scaleV3.y, distance / scaleV3.z) * 0.003f;
		koef = glm::clamp(koef, 1.00f, 100000.0f);
		shader->SetUniformMat4f("u_Scale", glm::scale(owner->m_Transform.m_ScaleMat4, koef));
		glDrawElements(GL_TRIANGLES, r3d->m_Mesh->m_Ib.m_count, GL_UNSIGNED_INT, nullptr);

	}
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_DEPTH_TEST);
}

void Renderer::RenderForPostEffect(Mesh* mesh, Shader* shader)
{
	if (mesh == nullptr) return;
	shader->Bind();
	mesh->m_Va.Bind();
	mesh->m_Ib.Bind();
	mesh->m_Vb.Bind();
	Renderer::s_Stats.m_Vertices += mesh->m_Ib.m_count;
	Renderer::s_Stats.m_DrawCalls++;
	glDrawElements(GL_TRIANGLES, mesh->m_Ib.m_count, GL_UNSIGNED_INT, nullptr);
	shader->UnBind();
	mesh->m_Ib.UnBind();
	glBindTextureUnit(0, Texture::s_WhiteTexture->m_RendererID);
}

void Renderer::Render() {
	int size[2];
	Renderer::s_Stats.Reset();
	if (Instancing::GetInstance()->m_DrawShadows > 0.5f) {
		{
			Timer t;
			FrameBuffer* shadowMap = Window::GetInstance().m_FrameBufferShadowMap;
			glfwGetWindowSize(Window::GetInstance().GetWindow(), &size[0], &size[1]);
			glViewport(0, 0, shadowMap->size.x, shadowMap->size.y);
			shadowMap->Bind();
			glClear(GL_DEPTH_BUFFER_BIT);
			Renderer::BakeShadows();
			Instancing::GetInstance()->BakeShadows();
			shadowMap->UnBind();
		}
		Renderer::s_Stats.m_ShadowRenderTime = Timer::s_OutTime;
		glViewport(0, 0, size[0], size[1]);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, Window::GetInstance().m_FrameBufferColor->m_Fbo);
	Renderer::Clear();
	Renderer::RenderScene();
	Window::GetInstance().GetScrollYOffset() = 0;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	Renderer::Clear();
	Instancing::GetInstance()->PrepareVertexAtrrib();

	{
		{
			Timer t;
			Renderer::RenderBloomEffect();
		}
		Renderer::s_Stats.m_BloomRenderTime = Timer::s_OutTime;
	}
	
	Shader* shader = Shader::Get("QuadFullScreen");
	{
		{
			Timer t;
			Renderer::Clear();
			glBindFramebuffer(GL_FRAMEBUFFER, Window::GetInstance().m_FrameBufferColor->m_Fbo);
			shader->Bind();
			glBindTexture(GL_TEXTURE_2D, Window::GetInstance().m_FrameBufferColor->m_Textures[0]);
			Renderer::RenderForPostEffect(Mesh::GetMesh("plane"), shader);
			glDisable(GL_DEPTH_TEST);
			Renderer::RenderCollision3D();
			Renderer::RenderCollision();
		}
		Renderer::s_Stats.m_CollisionRenderTime = Timer::s_OutTime;
	}
	{
		{
			Timer t;
			Renderer::RenderText();
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LESS);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			shader->UnBind();
		}
		Renderer::s_Stats.m_GuiRenderTime = Timer::s_OutTime;
	}
}

void Renderer::RenderScene() 
{
	{
		Timer t;
		Render2D();
		Render3D();
	}
	s_Stats.m_ObjectsRenderTime = Timer::s_OutTime;
}

void Renderer::Render2DObjects()
{
	if (Renderer::s_Objects2d.size() > 0 || Particle::s_Particles.size())
	{
		Batch& batch = Batch::GetInstance();
		batch.BeginGameObjects();


		{
			for (size_t i = 0; i < Renderer::s_Objects2d.size(); i++)
			{
				SpriteRenderer* r = Renderer::s_Objects2d[i];
				if (r->m_OwnerOfCom->m_Enable)// && sqrt(pow((go->position.x - Window::GetCamera().GetPosition().x), 2) + pow((go->position.y - Window::GetCamera().GetPosition().y), 2)) < 50 * Window::GetCamera().GetZoomLevel())
				{
					r->Render();
				}
			}
		}

		
		for (size_t i = 0; i < Particle::s_Particles.size(); i++)
		{
			Particle* p = Particle::s_Particles[i];
			if (p->Enable)
				Batch::GetInstance().Submit(p->pos, p->view, p->color, glm::vec2(p->scaleX, p->scaleY), p->texture);
		}
		batch.EndGameObjects();
	}
	Batch::GetInstance().FlushGameObjects(Batch::GetInstance().m_BasicShader);
}

void Renderer::Render3DObjects()
{
	if(s_PolygonMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	for each (Renderer3D* r in Renderer::s_Objects3d)
	{
		if (r->m_OwnerOfCom->m_Enable == true)
		{
			r->Render();
		}
	}

	Instancing::GetInstance()->Render();

	if (s_PolygonMode)
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
}

void Renderer::BakeShadows()
{
	for each (Renderer3D* r in Renderer::s_Objects3d)
	{
		if (r->m_OwnerOfCom->m_Enable == true)
		{
			r->BakeShadows();
		}
	}
	for each (Renderer3D* r in Renderer::s_Objects3dTransparent)
	{
		if (r->m_OwnerOfCom->m_Enable == true)
		{
			r->BakeShadows();
		}
	}
}

void Renderer::Render3D()
{
	if (Window::GetInstance().GetApp().m_Type == RenderType::TYPE_3D)
	{
		Render3DObjects();
		RenderLines();
		Render2DObjects();
		RenderTransparent();
	}
}

void Renderer::UpdateLightSpaceMatrices()
{
	for (DirectionalLight* light : DirectionalLight::s_DirLights)
	{
		light->UpdateLightMatrix();
	}
}

void Renderer::Render2D()
{
	if (Window::GetInstance().GetApp().m_Type == RenderType::TYPE_2D)
	{
		glDisable(GL_DEPTH_TEST);
		Render2DObjects();
		RenderLines();
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
	}
}

void Renderer::RenderCollision3D()
{
	size_t size = CubeCollider3D::s_Colliders.size();
	if (size > 0)
	{
		for (size_t i = 0; i < size; i++)
		{
			if (!CubeCollider3D::s_Colliders[i]->m_IsBoundingBox && RectangleCollider2D::s_IsVisible)
				CubeCollider3D::s_Colliders[i]->Render();
		}
	}
	if (size > 0)
	{
		for (size_t i = 0; i < size; i++)
		{
			if (CubeCollider3D::s_Colliders[i]->m_IsBoundingBox && Editor::GetInstance().isBoundingBoxesVisible)
				CubeCollider3D::s_Colliders[i]->Render();
		}
	}
	size = SphereCollider::s_Spheres.size();
	if (size > 0)
	{
		for (size_t i = 0; i < size; i++)
		{
			if (RectangleCollider2D::s_IsVisible)
				SphereCollider::s_Spheres[i]->Render();
		}
	}
}

void Renderer::RenderLines()
{
	Batch::GetInstance().BeginLines();
	uint32_t size = Line::s_Lines.size();
	for (uint32_t i = 0; i < size; i++)
	{
		if (Line::s_Lines[i]->m_Enable)
		{
			Batch::GetInstance().Submit(*Line::s_Lines[i]);
		}
	}
	Batch::GetInstance().EndLines();
	Batch::GetInstance().FlushLines(Batch::GetInstance().m_LineShader);
}

void Renderer::RenderText() 
{
	Batch::GetInstance().FlushText(Batch::GetInstance().m_TextShader);
}

void Renderer::RenderTransparent()
{
	for each (Renderer3D* r in Renderer::s_Objects3dTransparent)
	{
		if (r->m_OwnerOfCom->m_Enable == true)
		{
			r->Render();
		}
	}
}

void Renderer::RenderCollision()
{
	if (RectangleCollider2D::s_IsVisible == true)
	{
		Batch::GetInstance().BeginGameObjects();
		for (unsigned int i = 0; i < RectangleCollider2D::s_Collision2d.size(); i++) 
		{
			RectangleCollider2D* col = RectangleCollider2D::s_Collision2d[i];
			if (col != nullptr && col->m_Enable == true)
			{
				Batch::GetInstance().Submit(*col);
			}
		}
		Batch::GetInstance().EndGameObjects();
		Batch::GetInstance().FlushCollision(Batch::GetInstance().m_CollisionShader);
	}
}