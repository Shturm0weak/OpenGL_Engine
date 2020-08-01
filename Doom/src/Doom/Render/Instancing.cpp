#include "../pch.h"
#include "Instancing.h"

Doom::Instancing::Instancing()
{
}

void Doom::Instancing::Create(Mesh* mesh)
{
	glBuffers buf;
	buf.layout = mesh->layout;
	buf.vbo = mesh->vb;
	buf.vao = mesh->va;
	buf.ibo = mesh->ib;
	buffers.insert(std::make_pair(mesh, buf));
}

void Doom::Instancing::Render()
{
	Shader* shader = Shader::Get("Instancing3D");
	for (auto iter = instancedObjects.begin(); iter != instancedObjects.end(); iter++)
	{
		auto gliter = buffers.find(iter->first);
		if (gliter == buffers.end())
			continue;

		uint32_t objsSize = iter->second.size();
		if (objsSize == 0)
			continue;
		uint32_t sizeOfAttribs = 12 + 16;//pos, color,scale,(ambient,specular) 
		pos = new float[objsSize * sizeOfAttribs];
		for (size_t i = 0; i < objsSize; i++)
		{
			Renderer3D* r = iter->second[i];
			pos[i * sizeOfAttribs + 0] = r->GetOwnerOfComponent()->position.x;
			pos[i * sizeOfAttribs + 1] = r->GetOwnerOfComponent()->position.y;
			pos[i * sizeOfAttribs + 2] = r->GetOwnerOfComponent()->position.z;
			pos[i * sizeOfAttribs + 3] = r->GetOwnerOfComponent()->scaleValues.x;
			pos[i * sizeOfAttribs + 4] = r->GetOwnerOfComponent()->scaleValues.y;
			pos[i * sizeOfAttribs + 5] = r->GetOwnerOfComponent()->scaleValues.z;
			pos[i * sizeOfAttribs + 6] = r->color[0];
			pos[i * sizeOfAttribs + 7] = r->color[1];
			pos[i * sizeOfAttribs + 8] = r->color[2];
			pos[i * sizeOfAttribs + 9] = r->color[3];
			pos[i * sizeOfAttribs + 10] = r->mat.ambient;
			pos[i * sizeOfAttribs + 11] = r->mat.specular;

			float* view = glm::value_ptr(r->view);
			memcpy(&pos[i * sizeOfAttribs + 12], view, 16 * sizeof(float));
		}
		
		gliter->second.vboDynamic = new VertexBuffer(pos, sizeof(float) * sizeOfAttribs * objsSize);
		gliter->second.layoutDynamic = new VertexBufferLayout();
		gliter->second.vao->Bind();
		gliter->second.vboDynamic->Bind();
		gliter->second.layoutDynamic->Push<float>(3);
		gliter->second.layoutDynamic->Push<float>(3);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(2);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.layoutDynamic->Push<float>(4);
		gliter->second.vao->AddBuffer(*gliter->second.vboDynamic,*gliter->second.layoutDynamic,5,1);

		shader->Bind();
		glBindTextureUnit(0, iter->second[0]->diffuseTexture->m_RendererID);
		shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
		shader->SetUniform3f("u_LightPos", Renderer::Light->GetPositions().x, Renderer::Light->GetPositions().y, Renderer::Light->GetPositions().z);
		shader->SetUniform3fv("u_LightColor", Renderer::Light->GetComponentManager()->GetComponent<Irenderer>()->color);
		shader->SetUniform3fv("u_CameraPos", Window::GetCamera().GetPosition());
		shader->SetUniform1i("u_DiffuseTexture", 0);
		if (iter->second[0]->useNormalMap) {
			glBindTextureUnit(1, iter->second[0]->normalMapTexture->m_RendererID);
			shader->SetUniform1i("u_NormalMapTexture", 1);
		}
		shader->SetUniform1i("u_isNormalMapping", iter->second[0]->useNormalMap);
		Renderer::Vertices += gliter->first->meshSize;
		Renderer::DrawCalls++;
		shader->Bind();
		gliter->second.vao->Bind();
		gliter->second.ibo->Bind();
		gliter->second.vbo->Bind();
		Renderer::DrawCalls++;
		glDrawElementsInstanced(GL_TRIANGLES, gliter->second.ibo->GetCount(), GL_UNSIGNED_INT,0,iter->second.size());
		shader->UnBind();
		gliter->second.ibo->UnBind();
		delete[] pos;
		delete gliter->second.vboDynamic;
	}
}
