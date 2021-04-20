#include "Batch.h"
#include "../Core/Timer.h"
#include "Text/Gui.h"
#include "Core/Utils.h"

using namespace Doom;

Batch::Batch()
{
	InitText();
	InitGameObjects();
	InitLines();
	Logger::Success("Batch renderer has been initialized!");
}

Batch::~Batch()
{

}

Batch& Doom::Batch::GetInstance()
{
	static Batch instance;
	return instance;
}

void Batch::Submit(Character* c)
{

	if (m_TextB.m_TextureSlotsIndex > maxTextureSlots - 1)
	{
		EndText();
		FlushText(m_Shader);
		BeginText();
	}

	m_TextB.m_TextureIndex = 0.0f;

	//std::cout << "Batch " << c.position.x << "	" << c.position.y << std::endl;
	for (unsigned int i = 0; i < maxTextureSlots; i++)
	{
		if (m_TextB.m_TextureSlots[i] == c->m_Font->m_FontAtlas->m_RendererID)
		{
			m_TextB.m_TextureIndex = (float)i;
			break;
		}
	}

	if (m_TextB.m_TextureIndex == 0.0f)
	{
		m_TextB.m_TextureIndex = (float)m_TextB.m_TextureSlotsIndex;
		m_TextB.m_TextureSlots[m_TextB.m_TextureSlotsIndex] = c->m_Font->m_FontAtlas->m_RendererID;
		m_TextB.m_TextureSlotsIndex++;
	}

	m_TextB.m_BufferT->m_Vertex = glm::vec2((c->m_Mesh2D[0] * c->m_Scale.x + c->m_Position.x), ((c->m_Mesh2D[1]) * c->m_Scale.y + c->m_Position.y));
	m_TextB.m_BufferT->m_UV = glm::vec2(c->m_Mesh2D[2], c->m_Mesh2D[3]);
	m_TextB.m_BufferT->m_Static = c->m_IsRelatedToCam;
	m_TextB.m_BufferT->m_Color = c->m_Color;
	m_TextB.m_BufferT->m_IsGui = 0.;
	m_TextB.m_BufferT->m_TexIndex = m_TextB.m_TextureIndex;
	m_TextB.m_BufferT++;

	m_TextB.m_BufferT->m_Vertex = glm::vec2((c->m_Mesh2D[4] * c->m_Scale.x + c->m_Position.x), ((c->m_Mesh2D[5]) * c->m_Scale.y + c->m_Position.y));
	m_TextB.m_BufferT->m_UV = glm::vec2(c->m_Mesh2D[6], c->m_Mesh2D[7]);
	m_TextB.m_BufferT->m_Static = c->m_IsRelatedToCam;
	m_TextB.m_BufferT->m_Color = c->m_Color;
	m_TextB.m_BufferT->m_IsGui = 0.;
	m_TextB.m_BufferT->m_TexIndex = m_TextB.m_TextureIndex;
	m_TextB.m_BufferT++;

	m_TextB.m_BufferT->m_Vertex = glm::vec2((c->m_Mesh2D[8] * c->m_Scale.x + c->m_Position.x), ((c->m_Mesh2D[9]) * c->m_Scale.y + c->m_Position.y));
	m_TextB.m_BufferT->m_UV = glm::vec2(c->m_Mesh2D[10], c->m_Mesh2D[11]);
	m_TextB.m_BufferT->m_Static = c->m_IsRelatedToCam;
	m_TextB.m_BufferT->m_Color = c->m_Color;
	m_TextB.m_BufferT->m_IsGui = 0.;
	m_TextB.m_BufferT->m_TexIndex = m_TextB.m_TextureIndex;
	m_TextB.m_BufferT++;

	m_TextB.m_BufferT->m_Vertex = glm::vec2((c->m_Mesh2D[12] * c->m_Scale.x + c->m_Position.x), ((c->m_Mesh2D[13]) * c->m_Scale.y + c->m_Position.y));
	m_TextB.m_BufferT->m_UV = glm::vec2(c->m_Mesh2D[14], c->m_Mesh2D[15]);
	m_TextB.m_BufferT->m_Static = c->m_IsRelatedToCam;
	m_TextB.m_BufferT->m_Color = c->m_Color;
	m_TextB.m_BufferT->m_IsGui = 0.;
	m_TextB.m_BufferT->m_TexIndex = m_TextB.m_TextureIndex;
	m_TextB.m_BufferT++;

	m_TIndexCount += 6;
}

void Doom::Batch::Submit(glm::mat4 pos, glm::mat4 view, glm::vec4 color, glm::vec2 scale, Texture * texture)
{

	float mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
	};

	if (m_GoB.m_TextureSlotsIndex > maxTextureSlots - 1 || m_GIndexCount >= RENDERER_INDICES_SIZE) 
	{
		Batch::GetInstance().EndGameObjects();
		Batch::GetInstance().FlushGameObjects(Batch::GetInstance().m_BasicShader);
		Batch::GetInstance().BeginGameObjects();
	}
	m_GoB.m_TextureIndex = 0.0f;

	if (texture != nullptr) 
	{
		for (unsigned int i = 0; i < maxTextureSlots; i++)
		{
			if (m_GoB.m_TextureSlots[i] == texture->m_RendererID) 
			{
				m_GoB.m_TextureIndex = (float)i;
				break;
			}

		}
		if (m_GoB.m_TextureIndex == 0.0f) 
		{
			m_GoB.m_TextureIndex = (float)m_GoB.m_TextureSlotsIndex;
			m_GoB.m_TextureSlots[m_GoB.m_TextureSlotsIndex] = texture->m_RendererID;
			m_GoB.m_TextureSlotsIndex++;
		}
	}

	glm::vec3 posv = Utils::GetPosition(pos);
	m_GoB.m_BufferG->m_Vertex = glm::vec2(mesh2D[0] * scale.x, mesh2D[1] * scale.y);
	m_GoB.m_BufferG->m_UV = glm::vec2(mesh2D[2], mesh2D[3]);
	m_GoB.m_BufferG->m_Color = color;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = view[0];
	m_GoB.m_BufferG->m_RotationMat1 = view[1];
	m_GoB.m_BufferG->m_RotationMat2 = view[2];
	m_GoB.m_BufferG->m_RotationMat3 = view[3];
	m_GoB.m_BufferG->m_Pos = posv;
	m_GoB.m_BufferG++;

	m_GoB.m_BufferG->m_Vertex = glm::vec2(mesh2D[4] * scale.x, mesh2D[5] * scale.y);
	m_GoB.m_BufferG->m_UV = glm::vec2(mesh2D[6], mesh2D[7]);
	m_GoB.m_BufferG->m_Color = color;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = view[0];
	m_GoB.m_BufferG->m_RotationMat1 = view[1];
	m_GoB.m_BufferG->m_RotationMat2 = view[2];
	m_GoB.m_BufferG->m_RotationMat3 = view[3];
	m_GoB.m_BufferG->m_Pos = posv;
	m_GoB.m_BufferG++;

	m_GoB.m_BufferG->m_Vertex = glm::vec2(mesh2D[8] * scale.x, mesh2D[9] * scale.y);
	m_GoB.m_BufferG->m_UV = glm::vec2(mesh2D[10], mesh2D[11]);
	m_GoB.m_BufferG->m_Color = color;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = view[0];
	m_GoB.m_BufferG->m_RotationMat1 = view[1];
	m_GoB.m_BufferG->m_RotationMat2 = view[2];
	m_GoB.m_BufferG->m_RotationMat3 = view[3];
	m_GoB.m_BufferG->m_Pos = posv;
	m_GoB.m_BufferG++;

	m_GoB.m_BufferG->m_Vertex = glm::vec2(mesh2D[12] * scale.x, mesh2D[13] * scale.y);
	m_GoB.m_BufferG->m_UV = glm::vec2(mesh2D[14], mesh2D[15]);
	m_GoB.m_BufferG->m_Color = color;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = view[0];
	m_GoB.m_BufferG->m_RotationMat1 = view[1];
	m_GoB.m_BufferG->m_RotationMat2 = view[2];
	m_GoB.m_BufferG->m_RotationMat3 = view[3];
	m_GoB.m_BufferG->m_Pos = posv;
	m_GoB.m_BufferG++;

	m_GIndexCount += 6;
}

void Batch::Submit(float* mesh2D,glm::vec4 color,Texture* texture, glm::vec2 size, glm::vec2 pos,float radius) 
{

	if (m_GoB.m_TextureSlotsIndex > maxTextureSlots - 1)
	{
		EndText();
		FlushText(m_Shader);
		BeginText();
	}

	m_TextB.m_TextureIndex = 0.0f;

	if (texture != nullptr)
	{
		for (unsigned int i = 0; i < maxTextureSlots; i++)
		{
			if (m_TextB.m_TextureSlots[i] == texture->m_RendererID)
			{
				m_TextB.m_TextureIndex = (float)i;
				break;
			}

		}
		if (m_TextB.m_TextureIndex == 0.0f) 
		{
			m_TextB.m_TextureIndex = (float)m_TextB.m_TextureSlotsIndex;
			m_TextB.m_TextureSlots[m_TextB.m_TextureSlotsIndex] = texture->m_RendererID;
			m_TextB.m_TextureSlotsIndex++;
		}
	}

	m_TextB.m_BufferT->m_Vertex = glm::vec2((mesh2D[0]), mesh2D[1]);
	m_TextB.m_BufferT->m_UV = glm::vec2(0,0);
	m_TextB.m_BufferT->m_Static = true;
	m_TextB.m_BufferT->m_Color = color;
	m_TextB.m_BufferT->m_IsGui = 1.;
	m_TextB.m_BufferT->m_TexIndex = m_TextB.m_TextureIndex;
	m_TextB.m_BufferT++;

	m_TextB.m_BufferT->m_Vertex = glm::vec2((mesh2D[2]), (mesh2D[3]));
	m_TextB.m_BufferT->m_UV = glm::vec2(1.0f, 0.0f);
	m_TextB.m_BufferT->m_Static = true;
	m_TextB.m_BufferT->m_Color = color;
	m_TextB.m_BufferT->m_IsGui = 1.;
	m_TextB.m_BufferT->m_TexIndex = m_TextB.m_TextureIndex;
	m_TextB.m_BufferT++;

	m_TextB.m_BufferT->m_Vertex = glm::vec2((mesh2D[4]), (mesh2D[5]));
	m_TextB.m_BufferT->m_UV = glm::vec2(1.0f, 1.0f);
	m_TextB.m_BufferT->m_Static = true;
	m_TextB.m_BufferT->m_Color = color;
	m_TextB.m_BufferT->m_IsGui = 1.;
	m_TextB.m_BufferT->m_TexIndex = m_TextB.m_TextureIndex;
	m_TextB.m_BufferT++;

	m_TextB.m_BufferT->m_Vertex = glm::vec2((mesh2D[6]), ((mesh2D[7])));
	m_TextB.m_BufferT->m_UV = glm::vec2(0.0f, 1.0f);
	m_TextB.m_BufferT->m_Static = true;
	m_TextB.m_BufferT->m_Color = color;
	m_TextB.m_BufferT->m_IsGui = 1.;
	m_TextB.m_BufferT->m_TexIndex = m_TextB.m_TextureIndex;
	m_TextB.m_BufferT++;

	m_TIndexCount += 6;
}

#include "../Core/Editor.h"

void Doom::Batch::Submit(Line & line)
{
	if (m_LIndexCount >= RENDERER_MAX_SPRITES)
	{
		EndLines();
		FlushLines(m_LineShader);
		BeginLines();
	}

	m_LinesB.m_BufferL->m_Vertex = glm::vec3(line.m_Verteces[0], line.m_Verteces[1], line.m_Verteces[2]);
	m_LinesB.m_BufferL->m_Color = line.m_Color;

	m_LinesB.m_BufferL++;

	m_LinesB.m_BufferL->m_Vertex = glm::vec3(line.m_Verteces[3], line.m_Verteces[4],line.m_Verteces[5]);
	m_LinesB.m_BufferL->m_Color = line.m_Color;

	m_LinesB.m_BufferL++;

	m_LIndexCount += 2;
}


void Doom::Batch::Submit(glm::vec4 color, float* mesh2D)
{

	if (m_LIndexCount >= RENDERER_INDICES_SIZE)
	{
		EndLines();
		FlushLines(m_LineShader);
		BeginLines();
		m_LIndexCount = 0;
	}

	m_LinesB.m_BufferL->m_Vertex = glm::vec3(mesh2D[0], mesh2D[1], mesh2D[2]);
	m_LinesB.m_BufferL->m_Color = color;

	m_LinesB.m_BufferL++;

	m_LinesB.m_BufferL->m_Vertex = glm::vec3(mesh2D[3], mesh2D[4], mesh2D[5]);
	m_LinesB.m_BufferL->m_Color = color;

	m_LinesB.m_BufferL++;

	m_LIndexCount += 3;
}

void Batch::Submit(SpriteRenderer* c)
{

	if (m_GoB.m_TextureSlotsIndex > maxTextureSlots - 1 || m_GIndexCount >= RENDERER_INDICES_SIZE)
	{
		Batch::GetInstance().EndGameObjects();
		Batch::GetInstance().FlushGameObjects(Batch::GetInstance().m_BasicShader);
		Batch::GetInstance().BeginGameObjects();
	}
	m_GoB.m_TextureIndex = 0.0f;

	bool isFound = false;
	if (c->m_Texture != nullptr)
	{
		auto iter = std::find(m_GoB.m_TextureSlots.begin(), m_GoB.m_TextureSlots.end(), c->m_Texture->m_RendererID);
		if (iter != m_GoB.m_TextureSlots.end())
		{
			m_GoB.m_TextureIndex = (float)(iter - m_GoB.m_TextureSlots.begin());
			isFound = true;
		}
		if (!isFound)
		{
			m_GoB.m_TextureIndex = (float)m_GoB.m_TextureSlotsIndex;
			m_GoB.m_TextureSlots[m_GoB.m_TextureSlotsIndex] = c->m_Texture->m_RendererID;
			m_GoB.m_TextureSlotsIndex++;
		}
	}

	Transform& trOfR = c->m_OwnerOfCom->m_Transform;

	glm::vec2 scale = c->m_OwnerOfCom->GetScale();
	glm::vec3 pos = c->m_OwnerOfCom->GetPosition();
	m_GoB.m_BufferG->m_Vertex = glm::vec2(c->m_Mesh2D[0] * scale[0], c->m_Mesh2D[1] * scale[1]);
	m_GoB.m_BufferG->m_UV = glm::vec2(c->m_Mesh2D[2], c->m_Mesh2D[3]);
	m_GoB.m_BufferG->m_Color = c->m_Color;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = trOfR.m_ViewMat4[0];
	m_GoB.m_BufferG->m_RotationMat1 = trOfR.m_ViewMat4[1];
	m_GoB.m_BufferG->m_RotationMat2 = trOfR.m_ViewMat4[2];
	m_GoB.m_BufferG->m_RotationMat3 = trOfR.m_ViewMat4[3];
	m_GoB.m_BufferG->m_Pos = pos;
	m_GoB.m_BufferG++;

	m_GoB.m_BufferG->m_Vertex = glm::vec2(c->m_Mesh2D[4] * scale[0], c->m_Mesh2D[5] * scale[1]);
	m_GoB.m_BufferG->m_UV = glm::vec2(c->m_Mesh2D[6], c->m_Mesh2D[7]);
	m_GoB.m_BufferG->m_Color = c->m_Color;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = trOfR.m_ViewMat4[0];
	m_GoB.m_BufferG->m_RotationMat1 = trOfR.m_ViewMat4[1];
	m_GoB.m_BufferG->m_RotationMat2 = trOfR.m_ViewMat4[2];
	m_GoB.m_BufferG->m_RotationMat3 = trOfR.m_ViewMat4[3];
	m_GoB.m_BufferG->m_Pos = pos;
	m_GoB.m_BufferG++;

	m_GoB.m_BufferG->m_Vertex = glm::vec2(c->m_Mesh2D[8] * scale[0], c->m_Mesh2D[9] * scale[1]);
	m_GoB.m_BufferG->m_UV = glm::vec2(c->m_Mesh2D[10], c->m_Mesh2D[11]);
	m_GoB.m_BufferG->m_Color = c->m_Color;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = trOfR.m_ViewMat4[0];
	m_GoB.m_BufferG->m_RotationMat1 = trOfR.m_ViewMat4[1];
	m_GoB.m_BufferG->m_RotationMat2 = trOfR.m_ViewMat4[2];
	m_GoB.m_BufferG->m_RotationMat3 = trOfR.m_ViewMat4[3];
	m_GoB.m_BufferG->m_Pos = pos;
	m_GoB.m_BufferG++;

	m_GoB.m_BufferG->m_Vertex = glm::vec2(c->m_Mesh2D[12] * scale[0], c->m_Mesh2D[13] * scale[1]);
	m_GoB.m_BufferG->m_UV = glm::vec2(c->m_Mesh2D[14], c->m_Mesh2D[15]);
	m_GoB.m_BufferG->m_Color = c->m_Color;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = trOfR.m_ViewMat4[0];
	m_GoB.m_BufferG->m_RotationMat1 = trOfR.m_ViewMat4[1];
	m_GoB.m_BufferG->m_RotationMat2 = trOfR.m_ViewMat4[2];
	m_GoB.m_BufferG->m_RotationMat3 = trOfR.m_ViewMat4[3];
	m_GoB.m_BufferG->m_Pos = pos;
	m_GoB.m_BufferG++;

	m_GIndexCount += 6;
}

void Batch::Submit(RectangleCollider2D& c) 
{
	glm::mat4 mvp = c.m_OwnerOfCom->m_ComponentManager.GetComponent<Transform>()->m_PosMat4 * c.m_OwnerOfCom->m_ComponentManager.GetComponent<Transform>()->m_ViewMat4;
	if (m_GoB.m_TextureSlotsIndex > maxTextureSlots - 1)
	{
		EndGameObjects();
		FlushGameObjects(m_Shader);
		BeginGameObjects();
		Logger::Warning("texture limit", "Batch renderer", "Sprites");
	}

	m_GoB.m_TextureIndex = 0.0f;

	glm::vec3 scaleVal = c.m_OwnerOfCom->GetScale();

	m_GoB.m_BufferG->m_Vertex = glm::vec2(c.m_Vertices[0] * scaleVal[0], c.m_Vertices[1] * scaleVal[1]);
	m_GoB.m_BufferG->m_UV = glm::vec2(0, 0);
	m_GoB.m_BufferG->m_Color = COLORS::White;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = mvp[0];
	m_GoB.m_BufferG->m_RotationMat1 = mvp[1];
	m_GoB.m_BufferG->m_RotationMat2 = mvp[2];
	m_GoB.m_BufferG->m_RotationMat3 = mvp[3];
	m_GoB.m_BufferG++;

	m_GoB.m_BufferG->m_Vertex = glm::vec2(c.m_Vertices[2] * scaleVal[0], c.m_Vertices[3] * scaleVal[1]);
	m_GoB.m_BufferG->m_UV = glm::vec2(0, 0);
	m_GoB.m_BufferG->m_Color = COLORS::White;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = mvp[0];
	m_GoB.m_BufferG->m_RotationMat1 = mvp[1];
	m_GoB.m_BufferG->m_RotationMat2 = mvp[2];
	m_GoB.m_BufferG->m_RotationMat3 = mvp[3];
	m_GoB.m_BufferG++;

	m_GoB.m_BufferG->m_Vertex = glm::vec2(c.m_Vertices[4] * scaleVal[0], c.m_Vertices[5] * scaleVal[1]);
	m_GoB.m_BufferG->m_UV = glm::vec2(0, 0);
	m_GoB.m_BufferG->m_Color = COLORS::White;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = mvp[0];
	m_GoB.m_BufferG->m_RotationMat1 = mvp[1];
	m_GoB.m_BufferG->m_RotationMat2 = mvp[2];
	m_GoB.m_BufferG->m_RotationMat3 = mvp[3];
	m_GoB.m_BufferG++;

	m_GoB.m_BufferG->m_Vertex = glm::vec2(c.m_Vertices[6] * scaleVal[0], c.m_Vertices[7] * scaleVal[1]);
	m_GoB.m_BufferG->m_UV = glm::vec2(0, 0);
	m_GoB.m_BufferG->m_Color = COLORS::White;
	m_GoB.m_BufferG->m_TexIndex = m_GoB.m_TextureIndex;
	m_GoB.m_BufferG->m_RotationMat0 = mvp[0];
	m_GoB.m_BufferG->m_RotationMat1 = mvp[1];
	m_GoB.m_BufferG->m_RotationMat2 = mvp[2];
	m_GoB.m_BufferG->m_RotationMat3 = mvp[3];
	m_GoB.m_BufferG++;

	m_GIndexCount += 6;
	m_Shader = c.s_Shader;
}

void Batch::FlushGameObjects(Shader * shader)
{
	glBindVertexArray(m_GoB.m_Vao);
	m_GoB.m_Ibo.Bind();
	shader->Bind();
	int samplers[32];
	for (unsigned int i = 2; i < m_GoB.m_TextureSlotsIndex; i++)
	{
		glBindTextureUnit(i, m_GoB.m_TextureSlots[i]);
	}
	for (unsigned int i = 0; i < maxTextureSlots; i++)
	{
		samplers[i] = i;
	}

	shader->SetUniform1iv("u_Texture", samplers);
	shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().m_ViewProjectionMat4);
	shader->SetUniform1f("Brightness", Renderer::s_Brightness);
	glDrawElements(GL_TRIANGLES, m_GIndexCount, GL_UNSIGNED_INT, NULL);
	Renderer::s_Stats.m_DrawCalls++;
	Renderer::s_Stats.m_Vertices += m_GIndexCount;
	shader->UnBind();
	m_GoB.m_Ibo.UnBind();
	m_GoB.m_TextureSlotsIndex = 2;
	m_GIndexCount = 0;
	for (unsigned int i = m_GoB.m_TextureSlotsIndex; i < 32; i++)
	{
		m_GoB.m_TextureSlots[i] = 0;
		glBindTextureUnit(i, 0);
	}
	glDisable(GL_TEXTURE_2D_ARRAY);
}

void Batch::FlushCollision(Shader * shader)
{
	glBindVertexArray(m_GoB.m_Vao);
	m_GoB.m_Ibo.Bind();
	shader->Bind();
	shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().GetViewProjectionMatrix());
	shader->SetUniform4fv("U_Color", COLORS::Green);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0f);
	glDrawElements(GL_TRIANGLES, m_GIndexCount, GL_UNSIGNED_INT, NULL);
	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Renderer::s_Stats.m_DrawCalls++;
	Renderer::s_Stats.m_Vertices += m_GIndexCount;
	shader->UnBind();
	m_GoB.m_Ibo.UnBind();
	m_GIndexCount = 0;
}

void Batch::BeginText() 
{
	m_TextB.m_BufferT = m_TextB.m_BufferPtrT;
	m_TIndexCount = 0;
}

void Batch::EndText() 
{
	uint32_t dataSize = (uint8_t*)m_TextB.m_BufferT - (uint8_t*)m_TextB.m_BufferPtrT;
	glBindBuffer(GL_ARRAY_BUFFER, m_TextB.m_Vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_TextB.m_BufferPtrT);
}

void Batch::BeginGameObjects()
{
	m_GoB.m_BufferG = m_GoB.m_BufferPtrG;
	m_GIndexCount = 0;
}

void Batch::EndGameObjects()
{
	uint32_t dataSize = (uint8_t*)m_GoB.m_BufferG - (uint8_t*)m_GoB.m_BufferPtrG;
	glBindBuffer(GL_ARRAY_BUFFER, m_GoB.m_Vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_GoB.m_BufferPtrG);
}

void Doom::Batch::BeginLines()
{
	m_LinesB.m_BufferL = m_LinesB.m_BufferPtrL;
	m_LIndexCount = 0;
}

void Doom::Batch::EndLines()
{
	uint32_t dataSize = (uint8_t*)m_LinesB.m_BufferL - (uint8_t*)m_LinesB.m_BufferPtrL;
	glBindBuffer(GL_ARRAY_BUFFER, m_LinesB.m_Vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, m_LinesB.m_BufferPtrL);
}

void Batch::FlushText(Shader* shader)
{
	glBindVertexArray(m_TextB.m_Vao);
	m_TextB.m_Ibo.Bind();
	shader->Bind();
	int samplers[32];
	for (unsigned int i = 0; i < m_TextB.m_TextureSlotsIndex; i++)
	{
		glBindTextureUnit(i, m_TextB.m_TextureSlots[i]);
	}
	for (unsigned int i = 0; i < maxTextureSlots; i++)
	{
		samplers[i] = i;
	}

	Gui& g = Gui::GetInstance();
	shader->SetUniform1iv("u_Texture", samplers);
	shader->SetUniformMat4f("u_Projection", g.m_ViewProjecTionMat4RelatedToCamera);
	shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().GetViewProjectionMatrix());
	shader->SetUniform2fv("u_offset",       g.m_TextProps.m_ShadowOffset);
	shader->SetUniform1f("u_width",         g.m_TextProps.m_Width);
	shader->SetUniform1f("u_edge",          g.m_TextProps.m_Edge);
	shader->SetUniform1f("u_borderwidth",   g.m_TextProps.m_BorderWidth);
	shader->SetUniform1f("u_borderedge",    g.m_TextProps.m_BorderEdge);
	shader->SetUniform4fv("u_outlineColor", g.m_TextProps.m_OutLineColor);

	glDrawElements(GL_TRIANGLES, m_TIndexCount, GL_UNSIGNED_INT, NULL);
	Renderer::s_Stats.m_DrawCalls++;
	Renderer::s_Stats.m_Vertices += m_TIndexCount;
	shader->UnBind();
	m_TextB.m_Ibo.UnBind();
	glBindVertexArray(0);
	m_TextB.m_TextureSlotsIndex = 1;
	m_TextB.m_TextureSlots[0] = Texture::s_WhiteTexture->m_RendererID;
	for (unsigned int i = m_TextB.m_TextureSlotsIndex; i < 32; i++)
	{
		m_TextB.m_TextureSlots[i] = 0;
		glBindTextureUnit(i, 0);
	}
}

void Doom::Batch::FlushLines(Shader * shader)
{
	glBindVertexArray(m_LinesB.m_Vao);
	m_LinesB.m_Ibo.Bind();
	shader->Bind();
	shader->SetUniformMat4f("u_ViewProjection", Window::GetInstance().GetCamera().GetViewProjectionMatrix());
	glLineWidth(Line::s_Width);
	glDrawElements(GL_LINES, m_LIndexCount, GL_UNSIGNED_INT, nullptr);
	glLineWidth(1.0f);
	Renderer::s_Stats.m_DrawCalls++;
	Renderer::s_Stats.m_Vertices += m_LIndexCount;
	shader->UnBind();
	m_LinesB.m_Ibo.UnBind();
	glBindVertexArray(0);
}

void Batch::InitText()
{
	glGenVertexArrays(1, &m_TextB.m_Vao);
	glBindVertexArray(m_TextB.m_Vao);

	glGenBuffers(1, &m_TextB.m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_TextB.m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(TextVertex) * 4 * RENDERER_MAX_SPRITES, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(m_TextB.m_Vao, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (const GLvoid*)offsetof(TextVertex, m_Vertex));

	glEnableVertexArrayAttrib(m_TextB.m_Vao, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (const GLvoid*)offsetof(TextVertex, m_UV));

	glEnableVertexArrayAttrib(m_TextB.m_Vao, 2);
	glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(TextVertex), (const GLvoid*)offsetof(TextVertex, m_Static));

	glEnableVertexArrayAttrib(m_TextB.m_Vao, 3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (const GLvoid*)offsetof(TextVertex, m_Color));

	glEnableVertexArrayAttrib(m_TextB.m_Vao, 4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (const GLvoid*)offsetof(TextVertex, m_IsGui));

	glEnableVertexArrayAttrib(m_TextB.m_Vao, 5);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(TextVertex), (const GLvoid*)offsetof(TextVertex, m_TexIndex));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int indices[RENDERER_INDICES_SIZE];

	unsigned int offset = 0;
	for (unsigned int i = 0; i < RENDERER_INDICES_SIZE; i += 6)
	{
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;
		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;

		offset += 4;
	}

	m_TextB.m_BufferPtrT = new TextVertex[RENDERER_MAX_SPRITES];
	m_TextB.m_Ibo.Init(indices, RENDERER_INDICES_SIZE);
	glBindVertexArray(0);

	m_TextB.m_TextureSlots[0] = Texture::s_WhiteTexture->m_RendererID;
	for (unsigned int i = 1; i < 32; i++)
	{
		m_TextB.m_TextureSlots[i] = 0;
	}

}

void Batch::InitGameObjects()
{
	glGenVertexArrays(1, &m_GoB.m_Vao);
	glBindVertexArray(m_GoB.m_Vao);

	glGenBuffers(1, &m_GoB.m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_GoB.m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GOVertex) * 4 * RENDERER_MAX_SPRITES, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(m_GoB.m_Vao, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_Vertex));

	glEnableVertexArrayAttrib(m_GoB.m_Vao, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_UV));

	glEnableVertexArrayAttrib(m_GoB.m_Vao, 3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_Color));

	glEnableVertexArrayAttrib(m_GoB.m_Vao, 4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_TexIndex));

	glEnableVertexArrayAttrib(m_GoB.m_Vao, 5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_RotationMat0));

	glEnableVertexArrayAttrib(m_GoB.m_Vao, 6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_RotationMat1));

	glEnableVertexArrayAttrib(m_GoB.m_Vao, 7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_RotationMat2));

	glEnableVertexArrayAttrib(m_GoB.m_Vao, 8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_RotationMat3));

	glEnableVertexArrayAttrib(m_GoB.m_Vao, 9);
	glVertexAttribPointer(9, 3, GL_FLOAT, GL_FALSE, sizeof(GOVertex), (const GLvoid*)offsetof(GOVertex, m_Pos));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int indices[RENDERER_INDICES_SIZE];

	unsigned int offset = 0;
	for (unsigned int i = 0; i < RENDERER_INDICES_SIZE; i += 6)
	{
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;
		indices[i + 2] = offset + 2;
		indices[i + 3] = offset + 2;
		indices[i + 4] = offset + 3;
		indices[i + 5] = offset + 0;

		offset += 4;
	}

	m_GoB.m_BufferPtrG = new GOVertex[RENDERER_MAX_SPRITES * 4];
	m_GoB.m_Ibo.Init(indices, RENDERER_INDICES_SIZE);
	glBindVertexArray(0);

	m_GoB.m_TextureSlots[0] = Texture::s_WhiteTexture->m_RendererID;
	for (unsigned int i = 1; i < 32; i++)
	{
		m_GoB.m_TextureSlots[i] = 0;
	}
}

void Doom::Batch::InitLines()
{
	glGenVertexArrays(1, &m_LinesB.m_Vao);
	glBindVertexArray(m_LinesB.m_Vao);

	glGenBuffers(1, &m_LinesB.m_Vbo);
	glBindBuffer(GL_ARRAY_BUFFER, m_LinesB.m_Vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexLine) * 2 * RENDERER_MAX_SPRITES, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(m_LinesB.m_Vao, 0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, m_Vertex));

	glEnableVertexArrayAttrib(m_LinesB.m_Vao, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, m_Color));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int indices[RENDERER_INDICES_SIZE];

	unsigned int offset = 0;
	for (unsigned int i = 0; i < RENDERER_INDICES_SIZE; i += 2)
	{
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;

		offset += 2;
	}

	m_LinesB.m_Ibo.Init(indices, RENDERER_INDICES_SIZE);
	m_LinesB.m_BufferPtrL = new VertexLine[RENDERER_MAX_SPRITES];
	glBindVertexArray(0);
}