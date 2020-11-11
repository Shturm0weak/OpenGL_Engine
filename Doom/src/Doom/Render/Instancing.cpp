#include "../pch.h"
#include "Instancing.h"
#include "../Core/ThreadPool.h"

Doom::Instancing::Instancing()
{
	ready = new std::atomic<bool>[nThreads];
	for (size_t i = 0; i < nThreads; i++)
	{
		ready[i] = true;
	}
}

void Doom::Instancing::Create(Mesh* mesh)
{
	glBuffers buf;
	buf.layout = mesh->layout;
	buf.vbo = mesh->vb;
	buf.vao = mesh->va;
	buf.ibo = mesh->ib;
	buf.vboDynamic = new VertexBuffer(nullptr, 1,false);
	buf.layoutDynamic = new VertexBufferLayout();
	buffers.insert(std::make_pair(mesh, buf));
}

void Doom::Instancing::Render()
{
	if(shader == nullptr)
		shader = Shader::Get("Instancing3D");

	for (auto iter = instancedObjects.begin(); iter != instancedObjects.end(); iter++)
	{
		auto gliter = buffers.find(iter->first);
		if (gliter == buffers.end())
			continue;

		uint32_t objsSize = iter->second.size();
		if (objsSize == 0)
			continue;
		
		shader->Bind();
		glBindTextureUnit(0, iter->second[0]->diffuseTexture->m_RendererID);
		shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
		shader->SetUniform3fv("u_CameraPos", Window::GetCamera().GetPosition());

		int dlightSize = DirectionalLight::dirLights.size();
		shader->SetUniform1i("dLightSize", dlightSize);
		char buffer[64];
		for (int i = 0; i < dlightSize; i++)
		{	
			DirectionalLight* dl = DirectionalLight::dirLights[i];
			dl->dir = dl->GetOwnerOfComponent()->GetComponent<Transform>()->view * glm::vec4(0, 0, -1, 1);
			sprintf(buffer, "dirLights[%i].dir", i);
			shader->SetUniform3fv(buffer, dl->dir);
			sprintf(buffer, "dirLights[%i].color", i);
			shader->SetUniform3fv(buffer, dl->color);
			sprintf(buffer, "dirLights[%i].intensity", i);
			shader->SetUniform1f(buffer, dl->intensity);
		}

		int plightSize = PointLight::pLights.size();
		shader->SetUniform1i("pLightSize", plightSize);
		for (int i = 0; i < plightSize; i++)
		{
			PointLight* pl = PointLight::pLights[i];
			sprintf(buffer, "pointLights[%i].position", i);
			shader->SetUniform3fv(buffer, pl->GetOwnerOfComponent()->GetPosition());
			sprintf(buffer, "pointLights[%i].color", i);
			shader->SetUniform3fv(buffer, pl->color);
			sprintf(buffer, "pointLights[%i].constant", i);
			shader->SetUniform1f(buffer, pl->constant);
			sprintf(buffer, "pointLights[%i]._linear", i);
			shader->SetUniform1f(buffer, pl->linear);
			sprintf(buffer, "pointLights[%i].quadratic", i);
			shader->SetUniform1f(buffer, pl->quadratic);
		}

		shader->SetUniform1i("u_DiffuseTexture", 0);
		if (iter->second[0]->useNormalMap) {
			glBindTextureUnit(1, iter->second[0]->normalMapTexture->m_RendererID);
			shader->SetUniform1i("u_NormalMapTexture", 1);
		}
		shader->SetUniform1i("u_isNormalMapping", iter->second[0]->useNormalMap);

		shader->SetUniformMat4f("u_LightSpaceMatrix", DirectionalLight::GetLightSpaceMatrix());
		shader->SetUniform1f("u_DrawShadows", drawShadows);
		glBindTextureUnit(2, Window::GetCamera().frameBufferShadowMap->texture);
		shader->SetUniform1i("u_ShadowMap", 2);

		Renderer::Vertices += gliter->first->indicesSize * objsSize;
		Renderer::DrawCalls++;

		gliter->second.vao->Bind();
		gliter->second.ibo->Bind();
		gliter->second.vbo->Bind();

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

		gliter->second.vboDynamic->Bind();
		gliter->second.vboDynamic->Invalidate();
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeOfAttribs * objsSize, gliter->second.pos, GL_DYNAMIC_DRAW);
		gliter->second.layoutDynamic->Clear();
		gliter->second.vao->Bind();
		gliter->second.layoutDynamic->Push<float>(3);
		gliter->second.layoutDynamic->Push<float>(3);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(2);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.vao->AddBuffer(*gliter->second.vboDynamic, *gliter->second.layoutDynamic, 5, 1);

		glDrawElementsInstanced(GL_TRIANGLES, gliter->second.ibo->GetCount(), GL_UNSIGNED_INT, 0, iter->second.size());

		shader->UnBind();
		gliter->second.ibo->UnBind();
		gliter->second.vao->UnBind();
		gliter->second.vbo->UnBind();
		glBindTextureUnit(0, Texture::WhiteTexture->m_RendererID);
	}
}

void Doom::Instancing::BakeShadows()
{
	shader = Shader::Get("BakeShadowsInstancing");

	for (auto iter = instancedObjects.begin(); iter != instancedObjects.end(); iter++)
	{
		auto gliter = buffers.find(iter->first);
		if (gliter == buffers.end())
			continue;

		uint32_t objsSize = iter->second.size();
		if (objsSize == 0)
			continue;

		shader->Bind();

		shader->SetUniformMat4f("lightSpaceMatrix", DirectionalLight::GetLightSpaceMatrix());

		gliter->second.vao->Bind();
		gliter->second.ibo->Bind();
		gliter->second.vbo->Bind();

		std::unique_lock<std::mutex> lk(m);
		cv.wait(lk, [=] {
			for (uint32_t i = 0; i < nThreads; i++)
			{
				if (ready[i] == false) {
					return false;
				}
			}
			return true;
		});

		gliter->second.vboDynamic->Bind();
		gliter->second.vboDynamic->Invalidate();
		glBufferData(GL_ARRAY_BUFFER, 4 * sizeOfAttribs * objsSize, gliter->second.pos, GL_DYNAMIC_DRAW);
		gliter->second.layoutDynamic->Clear();
		gliter->second.vao->Bind();
		gliter->second.layoutDynamic->Push<float>(3);
		gliter->second.layoutDynamic->Push<float>(3);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(2);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.vao->AddBuffer(*gliter->second.vboDynamic, *gliter->second.layoutDynamic, 5, 1);

		glDrawElementsInstanced(GL_TRIANGLES, gliter->second.ibo->GetCount(), GL_UNSIGNED_INT, 0, iter->second.size());

		shader->UnBind();
		gliter->second.ibo->UnBind();
		gliter->second.vao->UnBind();
		gliter->second.vbo->UnBind();
		glBindTextureUnit(0, Texture::WhiteTexture->m_RendererID);
	}
	shader = nullptr;
}

void Doom::Instancing::PrepareVertexAtrrib()
{
	for (auto iter = instancedObjects.begin(); iter != instancedObjects.end(); iter++)
	{
		auto gliter = buffers.find(iter->first);
		if (gliter == buffers.end())
			continue;

		uint32_t objsSize = iter->second.size();
		if (objsSize == 0)
			continue;

		if (gliter->second.prevObjectSize != objsSize && gliter->second.pos == nullptr) {
			delete[] gliter->second.pos;
			gliter->second.pos = nullptr;
			gliter->second.pos = new float[objsSize * sizeOfAttribs];
		}

		gliter->second.prevObjectSize = objsSize;

		uint32_t dif = objsSize / nThreads;
		for (size_t k = 0; k < nThreads - 1; k++)
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
		ThreadPool::Instance()->enqueue([=] {
			{
				std::lock_guard lg(m);
				this->ready[k] = false;
			}
			for (uint32_t i = k * dif; i < k * dif + dif; i++)
			{
				Renderer3D* r = iter->second[i];
				float* posPtr = &gliter->second.pos[i * sizeOfAttribs];
				GameObject* owner = r->GetOwnerOfComponent();
				memcpy(posPtr, &owner->GetPosition()[0], 12);
				memcpy(&posPtr[3], &owner->GetScale()[0], 12);
				//r->color = color;
				memcpy(&posPtr[6], &r->color[0], 16);
				posPtr[10] = r->mat.ambient;
				posPtr[11] = r->mat.specular;
				float* view = glm::value_ptr(owner->GetComponent<Transform>()->view);
				memcpy(&posPtr[12], view, 64);
			}
			{
				std::lock_guard lg(m);
				this->ready[k] = true;
				cv.notify_all();
			}
		});
		}
		ThreadPool::Instance()->enqueue([=] {
			{
				std::lock_guard lg(m);
				this->ready[nThreads - 1] = false;
			}
			for (uint32_t i = (nThreads - 1) * dif; i < objsSize; i++)
			{
				Renderer3D* r = iter->second[i];
				float* posPtr = &gliter->second.pos[i * sizeOfAttribs];
				GameObject* owner = r->GetOwnerOfComponent();
				memcpy(posPtr, &owner->GetPosition()[0], 12);
				memcpy(&posPtr[3], &owner->GetScale()[0], 12);
				//r->color = COLORS::Blue;
				memcpy(&posPtr[6], &r->color[0], 16);
				posPtr[10] = r->mat.ambient;
				posPtr[11] = r->mat.specular;
				float* view = glm::value_ptr(owner->GetComponent<Transform>()->view);
				memcpy(&posPtr[12], view, 64);
			}
			{
				std::lock_guard lg(m);
				this->ready[nThreads - 1] = true;
				cv.notify_all();
			}
		});
	}
}
