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
		uint32_t sizeOfAttribs = 12 + 16;//pos, color,scale,(ambient,specular) + mat4 rotation
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
		gliter->second.layoutDynamic->Clear();
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
		shader->SetUniform3fv("u_CameraPos", Window::GetCamera().GetPosition());

		int dlightSize = DirectionalLight::dirLights.size();
		shader->SetUniform1i("dLightSize", dlightSize);
		for (int i = 0; i < dlightSize; i++)
		{
			char buffer[64];
			DirectionalLight* dl = DirectionalLight::dirLights[i];
			dl->dir = dl->GetOwnerOfComponent()->GetComponent<Irenderer>()->view * glm::vec4(0,0,-1,1);
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

		shader->SetUniform1i("u_DiffuseTexture", 0);
		if (iter->second[0]->useNormalMap) {
			glBindTextureUnit(1, iter->second[0]->normalMapTexture->m_RendererID);
			shader->SetUniform1i("u_NormalMapTexture", 1);
		}
		shader->SetUniform1i("u_isNormalMapping", iter->second[0]->useNormalMap);
		Renderer::Vertices += gliter->first->meshSize;
		Renderer::DrawCalls++;

		gliter->second.vao->Bind();
		gliter->second.ibo->Bind();
		gliter->second.vbo->Bind();

		glDrawElementsInstanced(GL_TRIANGLES, gliter->second.ibo->GetCount(), GL_UNSIGNED_INT,0,iter->second.size());

		shader->UnBind();
		gliter->second.ibo->UnBind();
		gliter->second.vao->UnBind();
		gliter->second.vbo->UnBind();

		delete[] pos;
		delete gliter->second.vboDynamic;
	}
}
