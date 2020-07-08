#include "../pch.h"
#include "Render3D.h"

void Doom::Renderer3D::LoadMesh(Mesh * mesh)
{
	this->mesh = mesh;
	delete layout;
	delete va;
	delete vb;
	delete ib;
	layout = new VertexBufferLayout();
	vb = new VertexBuffer(this->mesh->mesh, this->mesh->meshSize * sizeof(float));
	va = new VertexArray();
	ib = new IndexBuffer(this->mesh->indicesForNormals, this->mesh->indicesSize);
	layout->Push<float>(3);
	layout->Push<float>(3);
	layout->Push<float>(2);
	layout->Push<float>(3);
	layout->Push<float>(3);
	va->AddBuffer(*this->vb, *this->layout);
	va->UnBind();
	shader->UnBind();
	vb->UnBind();
	ib->UnBind();
}

Doom::Renderer3D::Renderer3D(GameObject* _owner)
{
	renderType = "3D";
	SetType("Renderer");
	owner = _owner;
	shader = new Shader("src/Shaders/Basic3D.shader");
	tr = owner->GetComponentManager()->GetComponent<Transform>();
	pos = translate(glm::mat4(1.f), glm::vec3(tr->position.x, tr->position.y, tr->position.z));
}

Doom::Renderer3D::~Renderer3D()
{
	delete layout;
	delete va;
	delete vb;
	delete ib;
	delete mesh;
}

void Doom::Renderer3D::Render()
{
	if (owner->name != "SkyBox") {
		if (mesh != nullptr) {
			this->shader->Bind();
			glBindTextureUnit(0, this->diffuseTexture->m_RendererID);
			this->pos = translate(glm::mat4(1.f), glm::vec3(tr->position.x, tr->position.y, tr->position.z));
			Window::GetCamera().RecalculateViewMatrix();
			this->shader->UploadUnifromMat4("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
			this->shader->SetUniformMat4f("u_Model", pos);
			this->shader->SetUniformMat4f("u_View", view);
			this->shader->SetUniformMat4f("u_Scale", scale);
			this->shader->SetUniform4fv("m_color", color);
			this->shader->SetUniform3fv("u_LightPos", glm::vec3(Renderer::Light->GetPositions().x, Renderer::Light->GetPositions().y, Renderer::Light->GetPositions().z));
			this->shader->SetUniform3fv("u_LightColor", glm::vec3(Renderer::Light->GetComponentManager()->GetComponent<Irenderer>()->color));
			this->shader->SetUniform3fv("u_CameraPos", Window::GetCamera().GetPosition());
			this->shader->SetUniform1f("u_Ambient", mat.ambient);
			this->shader->SetUniform1f("u_Specular", mat.specular);
			this->shader->SetUniform1i("u_DiffuseTexture", 0);
			if (useNormalMap) {
				glBindTextureUnit(1, this->normalMapTexture->m_RendererID);
				this->shader->SetUniform1i("u_NormalMapTexture", 1);
			}
			this->shader->SetUniform1i("u_isNormalMapping", useNormalMap);
			this->shader->Bind();
			va->Bind();
			ib->Bind();
			vb->Bind();
			Renderer::Vertices += mesh->meshSize / 2;
			Renderer::DrawCalls++;
			glDrawElements(GL_TRIANGLES, ib->GetCount(), GL_UNSIGNED_INT, nullptr);
			shader->UnBind();
			ib->UnBind();
		}
	}
	else {
		glDepthFunc(GL_LEQUAL);
		glActiveTexture(GL_TEXTURE0);
		glDisable(GL_CULL_FACE);
		glBindTexture(GL_TEXTURE_CUBE_MAP,diffuseTexture->m_RendererID);
		this->shader->Bind();
		this->pos = translate(glm::mat4(1.f), glm::vec3(tr->position.x, tr->position.y, tr->position.z));
		Window::GetCamera().RecalculateViewMatrix();
		this->shader->UploadUnifromMat4("u_ViewProjection", Window::GetCamera().GetProjectionMatrix());
		this->shader->SetUniformMat4f("u_Model", pos);
		this->shader->SetUniformMat4f("u_View", glm::mat4(glm::mat3(Window::GetCamera().GetViewMatrix())));
		this->shader->SetUniformMat4f("u_Scale", scale);
		this->shader->SetUniform1i("u_DiffuseTexture", 0);
		this->shader->Bind();
		va->Bind();
		ib->Bind();
		vb->Bind();
		Renderer::DrawCalls++;
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		shader->UnBind();
		ib->UnBind();
		diffuseTexture->UnBind();
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
	}
}
