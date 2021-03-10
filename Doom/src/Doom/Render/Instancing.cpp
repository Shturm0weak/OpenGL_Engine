#include "../pch.h"
#include "Instancing.h"
#include "../Core/ThreadPool.h"

Doom::Instancing::Instancing()
{
	m_Ready = new std::atomic<bool>[m_NThreads];
	for (size_t i = 0; i < m_NThreads; i++)
	{
		m_Ready[i] = true;
	}
}

void Doom::Instancing::Create(Mesh* mesh)
{
	m_Buffers.insert(std::make_pair(mesh, glBuffers()));
	m_Buffers.rbegin()->second.m_VboDynamic.Init(nullptr, 1, false);
}

void Doom::Instancing::Render()
{
	m_Shader = Shader::Get("Instancing3D");

	for (auto iter = m_InstancedObjects.begin(); iter != m_InstancedObjects.end(); iter++)
	{
		auto gliter = m_Buffers.find(iter->first);
		if (gliter == m_Buffers.end())
			continue;

		uint32_t objsSize = iter->second.size();
		if (objsSize == 0)
			continue;
		
		m_Shader->Bind();
		glBindTextureUnit(0, iter->second[0]->m_DiffuseTexture->m_RendererID);
		m_Shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().GetViewProjectionMatrix());
		m_Shader->SetUniform3fv("u_CameraPos", Window::GetInstance().GetCamera().GetPosition());

		int dlightSize = DirectionalLight::s_DirLights.size();
		m_Shader->SetUniform1i("dLightSize", dlightSize);
		char buffer[64];
		for (int i = 0; i < dlightSize; i++)
		{	
			DirectionalLight* dl = DirectionalLight::s_DirLights[i];
			dl->m_Dir = dl->GetOwnerOfComponent()->GetComponent<Transform>()->m_ViewMat4 * glm::vec4(0, 0, -1, 1);
			sprintf(buffer, "dirLights[%i].dir", i);
			m_Shader->SetUniform3fv(buffer, dl->m_Dir);
			sprintf(buffer, "dirLights[%i].color", i);
			m_Shader->SetUniform3fv(buffer, dl->m_Color);
			sprintf(buffer, "dirLights[%i].intensity", i);
			m_Shader->SetUniform1f(buffer, dl->m_Intensity);
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

		m_Shader->SetUniform1i("u_DiffuseTexture", 0);
		if (iter->second[0]->m_IsUsingNormalMap) 
		{
			glBindTextureUnit(1, iter->second[0]->m_NormalMapTexture->m_RendererID);
			m_Shader->SetUniform1i("u_NormalMapTexture", 1);
		}
		m_Shader->SetUniform1i("u_isNormalMapping", iter->second[0]->m_IsUsingNormalMap);

		m_Shader->SetUniformMat4f("u_LightSpaceMatrix", DirectionalLight::GetLightSpaceMatrix());
		m_Shader->SetUniform1f("u_DrawShadows", m_DrawShadows);
		glBindTextureUnit(2, Window::GetInstance().m_FrameBufferShadowMap->m_Textures[0]);
		m_Shader->SetUniform1i("u_ShadowMap", 2);
		m_Shader->SetUniform1f("Brightness", Renderer::s_Brightness);

		Renderer::s_Stats.m_Vertices += gliter->first->m_IndicesSize * objsSize;
		Renderer::s_Stats.m_DrawCalls++;

		//std::unique_lock<std::mutex> lk(m);
		//cv.wait(lk, [=] {
		//	for (uint32_t i = 0; i < nThreads; i++)
		//	{
		//		if (ready[i] == false) {
		//			return false;
		//		}
		//	}
		//	return true;
		//});

		gliter->first->m_Va.Bind();
		gliter->first->m_Ib.Bind();
		gliter->first->m_Vb.Bind();
		gliter->second.m_VboDynamic.Bind();
		gliter->second.m_VboDynamic.Invalidate();
		glBufferData(GL_ARRAY_BUFFER, 4 * m_SizeOfAttribs * objsSize, gliter->second.m_VertAttrib, GL_DYNAMIC_DRAW);
		gliter->second.m_LayoutDynamic.Clear();
		gliter->first->m_Va.Bind();
		gliter->second.m_LayoutDynamic.Push<float>(3);
		gliter->second.m_LayoutDynamic.Push<float>(3);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->second.m_LayoutDynamic.Push<float>(2);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->first->m_Va.AddBuffer(gliter->second.m_VboDynamic, gliter->second.m_LayoutDynamic, 5, 1);

		if (!iter->second[0]->m_IsCullingFace)
			glDisable(GL_CULL_FACE);
		glDrawElementsInstanced(GL_TRIANGLES, gliter->first->m_Ib.m_count, GL_UNSIGNED_INT, 0, iter->second.size());
		glEnable(GL_CULL_FACE);
		m_Shader->UnBind();
		gliter->first->m_Ib.UnBind();
		gliter->first->m_Va.UnBind();
		gliter->first->m_Vb.UnBind();
		glBindTextureUnit(0, Texture::s_WhiteTexture->m_RendererID);
	}
}

void Doom::Instancing::BakeShadows()
{
	m_Shader = Shader::Get("BakeShadowsInstancing");

	for (auto iter = m_InstancedObjects.begin(); iter != m_InstancedObjects.end(); iter++)
	{
		auto gliter = m_Buffers.find(iter->first);
		if (gliter == m_Buffers.end()) continue;

		uint32_t objsSize = iter->second.size();
		if (objsSize == 0) continue;

		m_Shader->Bind();

		m_Shader->SetUniformMat4f("lightSpaceMatrix", DirectionalLight::GetLightSpaceMatrix());

		gliter->first->m_Va.Bind();
		gliter->first->m_Ib.Bind();
		gliter->first->m_Vb.Bind();

		std::unique_lock<std::mutex> lk(World::GetInstance().m_Mtx);
		m_CondVar.wait(lk, [=] {
			for (uint32_t i = 0; i < m_NThreads; i++)
			{
				if (m_Ready[i] == false)
				{
					m_FinishPrepareVertAtribb = false;
					return false;
				}
			}
			m_FinishPrepareVertAtribb = true;
			return true;
		});


		gliter->second.m_VboDynamic.Bind();
		gliter->second.m_VboDynamic.Invalidate();
		glBufferData(GL_ARRAY_BUFFER, 4 * m_SizeOfAttribs * objsSize, gliter->second.m_VertAttrib, GL_DYNAMIC_DRAW);
		gliter->second.m_LayoutDynamic.Clear();
		gliter->first->m_Va.Bind();
		gliter->second.m_LayoutDynamic.Push<float>(3);
		gliter->second.m_LayoutDynamic.Push<float>(3);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->second.m_LayoutDynamic.Push<float>(2);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->second.m_LayoutDynamic.Push<float>(4);
		gliter->first->m_Va.AddBuffer(gliter->second.m_VboDynamic, gliter->second.m_LayoutDynamic, 5, 1);

		glDrawElementsInstanced(GL_TRIANGLES, gliter->first->m_Ib.m_count, GL_UNSIGNED_INT, 0, iter->second.size());

		m_Shader->UnBind();
		gliter->first->m_Ib.UnBind();
		gliter->first->m_Va.UnBind();
		gliter->first->m_Vb.UnBind();
		glBindTextureUnit(0, Texture::s_WhiteTexture->m_RendererID);
	}
}

void Doom::Instancing::PrepareVertexAtrrib()
{
	for (auto iter = m_InstancedObjects.begin(); iter != m_InstancedObjects.end(); iter++)
	{
		auto gliter = m_Buffers.find(iter->first);
		if (gliter == m_Buffers.end()) continue;

		uint32_t objsSize = iter->second.size();
		if (objsSize == 0) continue;

		if (gliter->second.m_PrevObjectSize != objsSize || gliter->second.m_VertAttrib == nullptr) //Probably I found a bug here && gliter->second.m_VertAttrib == nullptr)
		{
			delete[] gliter->second.m_VertAttrib;
			gliter->second.m_VertAttrib = nullptr;
			gliter->second.m_VertAttrib = new float[objsSize * m_SizeOfAttribs];
		}

		gliter->second.m_PrevObjectSize = objsSize;

		float dif = (float)objsSize / (float)m_NThreads;
		for (size_t k = 0; k < m_NThreads - 1; k++)
		{
			/*glm::vec4 color;
			if (k == 0)
				color = COLORS::Red;
			else if (k == 1)
				color = COLORS::Green;
			else if (k == 2)
				color = COLORS::Yellow;
			else if (k == 3)
				color = COLORS::Brown;
			else if (k == 4)
				color = COLORS::Silver;
			else if (k == 5)
				color = COLORS::DarkGray;
			else if (k == 6)
				color = COLORS::Orange;*/
		ThreadPool::GetInstance().Enqueue([=] {
			{
				std::lock_guard lg(World::GetInstance().m_Mtx);
				this->m_Ready[k] = false;
			}
			uint32_t thisSegmentOfObjectsV = k * dif + dif;
			for (uint32_t i = k * dif; i < thisSegmentOfObjectsV; i++)
			{
				//if (iter->second.size() <= i) continue;
				Renderer3D& r3d = *(iter->second[i]);
				//if (r3d.m_RenderTechnic != Renderer3D::RenderTechnic::Instancing) continue;
				GameObject& owner = *(r3d.GetOwnerOfComponent());
				if (owner.m_IsStatic && r3d.m_IsInitializedInInstancing) continue;
				float* posPtr = &gliter->second.m_VertAttrib[i * m_SizeOfAttribs];
				memcpy(posPtr, &owner.GetPosition()[0], 12);
				memcpy(&posPtr[3], &owner.GetScale()[0], 12);
				float* view = glm::value_ptr(owner.m_Transform.m_ViewMat4);
				memcpy(&posPtr[12], view, 64);
				//r->m_Color = color;
				memcpy(&posPtr[6], &r3d.m_Color[0], 16);
				posPtr[10] = r3d.m_Material.m_Ambient;
				posPtr[11] = r3d.m_Material.m_Specular;
				r3d.m_IsInitializedInInstancing = true;
			}
			{
				std::lock_guard lg(World::GetInstance().m_Mtx);
				this->m_Ready[k] = true;
				m_CondVar.notify_all();
			}
		});
		}
		ThreadPool::GetInstance().Enqueue([=] {
			{
				std::lock_guard lg(World::GetInstance().m_Mtx);
				this->m_Ready[m_NThreads - 1] = false;
			}
			for (uint32_t i = (m_NThreads - 1) * dif; i < objsSize; i++)
			{
				//if (iter->second.size() <= i) continue;
				Renderer3D& r3d = *(iter->second[i]);
				//if (r3d.m_RenderTechnic != Renderer3D::RenderTechnic::Instancing) continue;
				GameObject& owner = *(r3d.GetOwnerOfComponent());
				if (owner.m_IsStatic && r3d.m_IsInitializedInInstancing) continue;
				float* posPtr = &gliter->second.m_VertAttrib[i * m_SizeOfAttribs];
				memcpy(posPtr, &owner.GetPosition()[0], 12);
				memcpy(&posPtr[3], &owner.GetScale()[0], 12);
				float* view = glm::value_ptr(owner.m_Transform.m_ViewMat4);
				memcpy(&posPtr[12], view, 64);
				//r->m_Color = COLORS::Blue;
				memcpy(&posPtr[6], &r3d.m_Color[0], 16);
				posPtr[10] = r3d.m_Material.m_Ambient;
				posPtr[11] = r3d.m_Material.m_Specular;
				r3d.m_IsInitializedInInstancing = true;
			}
			{
				std::lock_guard lg(World::GetInstance().m_Mtx);
				this->m_Ready[m_NThreads - 1] = true;
				m_CondVar.notify_all();
			}
		});
	}
}

void Doom::Instancing::ShutDown()
{
	//for (auto buffer : m_Buffers)
	//{
	//	delete buffer.second.m_LayoutDynamic;
	//	delete buffer.second.m_VboDynamic;
	//}
}
