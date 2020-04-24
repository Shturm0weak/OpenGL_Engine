#include "Batch.h"
#include "../Core/Timer.h"

using namespace Doom;

Batch::Batch()
{
	std::cout << "Initialized Batch rendering" << std::endl;
	initText();
	initGameObjects();
	initLines();
}

void Batch::Init() {
	instance = new Batch();
}

Batch::~Batch()
{
	delete ibo;
	glDeleteBuffers(1, &vbo);
}

void Batch::Submit(Character& c)
{

	if (textureSlotsIndex > maxTextureSlots - 1) {
		End();
		flushText(shader);
		Begin();
	}

	textureIndex = 0.0f;

	//std::cout << "Batch " << c.position.x << "	" << c.position.y << std::endl;
	for (unsigned int i = 0; i < maxTextureSlots; i++)
	{
		if (textureSlots[i] == c.texture->m_RendererID) {
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f) {
		textureIndex = (float)textureSlotsIndex;
		textureSlots[textureSlotsIndex] = c.texture->m_RendererID;
		textureSlotsIndex++;
	}
	else {
		GtextureIndex = 0;
	}

	buffer->vertex = glm::vec2((c.mesh2D[0] * c._scale.x + c.position.x) * 0.019230769, ((c.mesh2D[1] - c.yoffset * 0.0079365079) * c._scale.y + c.position.y) * 0.019230769);
	buffer->textcoords = glm::vec2(c.mesh2D[2], c.mesh2D[3]);
	buffer->m_static = c.isRelatedToCam;
	buffer->m_color = c.color;
	buffer->isGui = 0.;
	buffer->texIndex = textureIndex;
	buffer++;

	buffer->vertex = glm::vec2((c.mesh2D[4] * c._scale.x + c.position.x) * 0.019230769, ((c.mesh2D[5] - c.yoffset * 0.0079365079) * c._scale.y + c.position.y) * 0.019230769);
	buffer->textcoords = glm::vec2(c.mesh2D[6], c.mesh2D[7]);
	buffer->m_static = c.isRelatedToCam;
	buffer->m_color = c.color;
	buffer->isGui = 0.;
	buffer->texIndex = textureIndex;
	buffer++;

	buffer->vertex = glm::vec2((c.mesh2D[8] * c._scale.x + c.position.x) * 0.019230769, ((c.mesh2D[9] - c.yoffset * 0.0079365079)* c._scale.y + c.position.y) * 0.019230769);
	buffer->textcoords = glm::vec2(c.mesh2D[10], c.mesh2D[11]);
	buffer->m_static = c.isRelatedToCam;
	buffer->m_color = c.color;
	buffer->isGui = 0.;
	buffer->texIndex = textureIndex;
	buffer++;

	buffer->vertex = glm::vec2((c.mesh2D[12] * c._scale.x + c.position.x) * 0.019230769, ((c.mesh2D[13] - c.yoffset * 0.0079365079) * c._scale.y + c.position.y) * 0.019230769);
	buffer->textcoords = glm::vec2(c.mesh2D[14], c.mesh2D[15]);
	buffer->m_static = c.isRelatedToCam;
	buffer->m_color = c.color;
	buffer->isGui = 0.;
	buffer->texIndex = textureIndex;
	buffer++;

	indexcount += 6;
}

void Batch::Submit(GuiItem& c) {
	buffer->vertex = glm::vec2((c.mesh2D[0]), c.mesh2D[1]);
	buffer->textcoords = glm::vec2(c.mesh2D[2], c.mesh2D[3]);
	buffer->m_static = true;
	buffer->m_color = c.color;
	buffer->isGui = 1.;
	buffer->texIndex = 0;
	buffer++;

	buffer->vertex = glm::vec2((c.mesh2D[4]), (c.mesh2D[5]));
	buffer->textcoords = glm::vec2(c.mesh2D[6], c.mesh2D[7]);
	buffer->m_static = true;
	buffer->m_color = c.color;
	buffer->isGui = 1.;
	buffer->texIndex = 0;
	buffer++;

	buffer->vertex = glm::vec2((c.mesh2D[8]), (c.mesh2D[9]));
	buffer->textcoords = glm::vec2(c.mesh2D[10], c.mesh2D[11]);
	buffer->m_static = true;
	buffer->m_color = c.color;
	buffer->isGui = 1.;
	buffer->texIndex = 0;
	buffer++;

	buffer->vertex = glm::vec2((c.mesh2D[12]), ((c.mesh2D[13])));
	buffer->textcoords = glm::vec2(c.mesh2D[14], c.mesh2D[15]);
	buffer->m_static = true;
	buffer->m_color = c.color;
	buffer->isGui = 1.;
	buffer->texIndex = 0;
	buffer++;

	indexcount += 6;
}

void Doom::Batch::Submit(Line & line)
{
	Lbuffer->m_vertex = glm::vec2(line.mesh2D[0], line.mesh2D[1]);
	Lbuffer->m_color = line.color;
	Lbuffer->MVPMat0 = line.MVP[0];
	Lbuffer->MVPMat1 = line.MVP[1];
	Lbuffer->MVPMat2 = line.MVP[2];
	Lbuffer->MVPMat3 = line.MVP[3];
	Lbuffer->rotationMat0 = line.view[0];
	Lbuffer->rotationMat1 = line.view[1];
	Lbuffer->rotationMat2 = line.view[2];
	Lbuffer->rotationMat3 = line.view[3];
	Lbuffer->m_color = line.color;

	Lbuffer++;

	Lbuffer->m_vertex = glm::vec2(line.mesh2D[2], line.mesh2D[3]);
	Lbuffer->m_color = line.color;
	Lbuffer->MVPMat0 = line.MVP[0];
	Lbuffer->MVPMat1 = line.MVP[1];
	Lbuffer->MVPMat2 = line.MVP[2];
	Lbuffer->MVPMat3 = line.MVP[3];
	Lbuffer->rotationMat0 = line.view[0];
	Lbuffer->rotationMat1 = line.view[1];
	Lbuffer->rotationMat2 = line.view[2];
	Lbuffer->rotationMat3 = line.view[3];
	Lbuffer->m_color = line.color;

	Lbuffer++;

	Lindexcount += 2;
}

void Batch::Submit(GameObject & c)
{

	if (GtextureSlotsIndex > maxTextureSlots - 1 || Gindexcount >= RENDERER_INDICES_SIZE) {
		Batch::GetInstance()->EndGameObjects();
		Batch::GetInstance()->flushGameObjects(Batch::GetInstance()->BasicShader);
		Batch::GetInstance()->BeginGameObjects();
	}
	GtextureIndex = 0.0f;

	if (c.texture != nullptr) {
		for (unsigned int i = 0; i < maxTextureSlots; i++)
		{
			if (GtextureSlots[i] == c.texture->m_RendererID) {
				GtextureIndex = (float)i;
				break;
			}

		}
		if (GtextureIndex == 0.0f) {
			GtextureIndex = (float)GtextureSlotsIndex;
			GtextureSlots[GtextureSlotsIndex] = c.texture->m_RendererID;
			GtextureSlotsIndex++;
		}
	}

	Gbuffer->vertex = glm::vec2(c.mesh2D[0] * c.scaleValues[0], c.mesh2D[1] * c.scaleValues[1]);
	Gbuffer->textcoords = glm::vec2(c.mesh2D[2], c.mesh2D[3]);
	Gbuffer->m_static = c.Static;
	Gbuffer->m_color = c.color;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = c.view[0];
	Gbuffer->rotationMat1 = c.view[1];
	Gbuffer->rotationMat2 = c.view[2];
	Gbuffer->rotationMat3 = c.view[3];
	Gbuffer->posMat0 = c.pos[0];
	Gbuffer->posMat1 = c.pos[1];
	Gbuffer->posMat2 = c.pos[2];
	Gbuffer->posMat3 = c.pos[3];
	Gbuffer->m_particle = c.isParticle;
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(c.mesh2D[4] * c.scaleValues[0], c.mesh2D[5] * c.scaleValues[1]);
	Gbuffer->textcoords = glm::vec2(c.mesh2D[6], c.mesh2D[7]);
	Gbuffer->m_static = c.Static;
	Gbuffer->m_color = c.color;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = c.view[0];
	Gbuffer->rotationMat1 = c.view[1];
	Gbuffer->rotationMat2 = c.view[2];
	Gbuffer->rotationMat3 = c.view[3];
	Gbuffer->posMat0 = c.pos[0];
	Gbuffer->posMat1 = c.pos[1];
	Gbuffer->posMat2 = c.pos[2];
	Gbuffer->posMat3 = c.pos[3];
	Gbuffer->m_particle = c.isParticle;
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(c.mesh2D[8] * c.scaleValues[0], c.mesh2D[9] * c.scaleValues[1]);
	Gbuffer->textcoords = glm::vec2(c.mesh2D[10], c.mesh2D[11]);
	Gbuffer->m_static = c.Static;
	Gbuffer->m_color = c.color;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = c.view[0];
	Gbuffer->rotationMat1 = c.view[1];
	Gbuffer->rotationMat2 = c.view[2];
	Gbuffer->rotationMat3 = c.view[3];
	Gbuffer->posMat0 = c.pos[0];
	Gbuffer->posMat1 = c.pos[1];
	Gbuffer->posMat2 = c.pos[2];
	Gbuffer->posMat3 = c.pos[3];
	Gbuffer->m_particle = c.isParticle;
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(c.mesh2D[12] * c.scaleValues[0], c.mesh2D[13] * c.scaleValues[1]);
	Gbuffer->textcoords = glm::vec2(c.mesh2D[14], c.mesh2D[15]);
	Gbuffer->m_static = c.Static;
	Gbuffer->m_color = c.color;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = c.view[0];
	Gbuffer->rotationMat1 = c.view[1];
	Gbuffer->rotationMat2 = c.view[2];
	Gbuffer->rotationMat3 = c.view[3];
	Gbuffer->posMat0 = c.pos[0];
	Gbuffer->posMat1 = c.pos[1];
	Gbuffer->posMat2 = c.pos[2];
	Gbuffer->posMat3 = c.pos[3];
	Gbuffer->m_particle = c.isParticle;
	Gbuffer++;

	Gindexcount += 6;
}

void Batch::Submit(Collision& c) {
	glm::mat4 mvp = c.owner->pos * c.owner->view;
	if (GtextureSlotsIndex > maxTextureSlots - 1) {
		EndGameObjects();
		flushGameObjects(shader);
		BeginGameObjects();
		std::cout << "max texture" << std::endl;
	}

	GtextureIndex = 0.0f;
	//std::cout << "Batch " << c.position.x << "	" << c.position.y << std::endl;
	float* scaleVal = c.owner->GetScale();
	Gbuffer->vertex = glm::vec2(c.positions[0] * scaleVal[0], c.positions[1] * scaleVal[1]);
	Gbuffer->textcoords = glm::vec2(c.positions[2], c.positions[3]);
	Gbuffer->m_static = c.Static;
	Gbuffer->m_color = COLORS::White;
	Gbuffer->isGui = 0.f;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = mvp[0];
	Gbuffer->rotationMat1 = mvp[1];
	Gbuffer->rotationMat2 = mvp[2];
	Gbuffer->rotationMat3 = mvp[3];
	Gbuffer->m_particle = c.owner->isParticle;
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(c.positions[4] * scaleVal[0], c.positions[5] * scaleVal[1]);
	Gbuffer->textcoords = glm::vec2(c.positions[6], c.positions[7]);
	Gbuffer->m_static = c.Static;
	Gbuffer->m_color = COLORS::White;
	Gbuffer->isGui = 0.f;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = mvp[0];
	Gbuffer->rotationMat1 = mvp[1];
	Gbuffer->rotationMat2 = mvp[2];
	Gbuffer->rotationMat3 = mvp[3];
	Gbuffer->m_particle = c.owner->isParticle;
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(c.positions[8] * scaleVal[0], c.positions[9] * scaleVal[1]);
	Gbuffer->textcoords = glm::vec2(c.positions[10], c.positions[11]);
	Gbuffer->m_static = c.Static;
	Gbuffer->m_color = COLORS::White;
	Gbuffer->isGui = 0.f;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = mvp[0];
	Gbuffer->rotationMat1 = mvp[1];
	Gbuffer->rotationMat2 = mvp[2];
	Gbuffer->rotationMat3 = mvp[3];
	Gbuffer->m_particle = c.owner->isParticle;
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(c.positions[12] * scaleVal[0], c.positions[13] * scaleVal[1]);
	Gbuffer->textcoords = glm::vec2(c.positions[14], c.positions[15]);
	Gbuffer->m_static = c.Static;
	Gbuffer->m_color = COLORS::White;
	Gbuffer->isGui = 0.f;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = mvp[0];
	Gbuffer->rotationMat1 = mvp[1];
	Gbuffer->rotationMat2 = mvp[2];
	Gbuffer->rotationMat3 = mvp[3];
	Gbuffer->m_particle = c.owner->isParticle;
	Gbuffer++;

	Gindexcount += 6;
	shader = c.shader;
}

void Batch::flushGameObjects(Shader * shader)
{
	glBindVertexArray(Gvao);
	Gibo->Bind();
	shader->Bind();
	int samplers[32];
	for (unsigned int i = 2; i < GtextureSlotsIndex; i++)
	{
		glBindTextureUnit(i, GtextureSlots[i]);

	}
	for (unsigned int i = 0; i < maxTextureSlots; i++)
	{
		samplers[i] = i;
	}

	shader->SetUniform1iv("u_Texture", samplers);
	shader->UploadUnifromMat4("u_Projection", ViewProjecTionRelatedToCamera);
	shader->UploadUnifromMat4("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());

	glDrawElements(GL_TRIANGLES, Gindexcount, GL_UNSIGNED_INT, NULL);
	Renderer::DrawCalls++;
	shader->UnBind();
	Gibo->UnBind();
	GtextureSlotsIndex = 2;
	Gindexcount = 0;
	for (unsigned int i = 2; i < 32; i++)
	{
		GtextureSlots[i] = 0;
		glBindTextureUnit(i, 0);
	}
}

void Batch::flushCollision(Shader * shader)
{
	glBindVertexArray(Gvao);
	Gibo->Bind();
	shader->Bind();
	shader->UploadUnifromMat4("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
	shader->SetUniform4fv("U_Color", COLORS::Green);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glLineWidth(3.0f);
	glDrawElements(GL_TRIANGLES, Gindexcount, GL_UNSIGNED_INT, NULL);
	glLineWidth(1.0f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	Renderer::DrawCalls++;
	shader->UnBind();
	Gibo->UnBind();
	Gindexcount = 0;
}

void Batch::Begin() {
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	buffer = (Vertex*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
}

void Batch::End() {
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Batch::BeginGameObjects()
{
	glBindBuffer(GL_ARRAY_BUFFER, Gvbo);
	Gbuffer = (Vertex*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
	isBegan = true;
}

void Batch::EndGameObjects()
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	isBegan = false;
}

void Doom::Batch::BeginLines()
{
	glBindBuffer(GL_ARRAY_BUFFER, Lvbo);
	Lbuffer = (VertexLine*)glMapBufferARB(GL_ARRAY_BUFFER_ARB, GL_WRITE_ONLY_ARB);
}

void Doom::Batch::EndLines()
{
	glUnmapBuffer(GL_ARRAY_BUFFER);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Batch::flushText(Shader* shader)
{
	glBindVertexArray(vao);
	ibo->Bind();
	shader->Bind();
	int samplers[32];
	for (unsigned int i = 0; i < textureSlotsIndex; i++)
	{
		glBindTextureUnit(i, textureSlots[i]);
	}
	for (unsigned int i = 0; i < maxTextureSlots; i++)
	{
		samplers[i] = i;
	}

	shader->SetUniform1iv("u_Texture", samplers);
	shader->UploadUnifromMat4("u_Projection", ViewProjecTionRelatedToCamera);
	shader->UploadUnifromMat4("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());

	glDrawElements(GL_TRIANGLES, indexcount, GL_UNSIGNED_INT, NULL);
	Renderer::DrawCalls++;
	shader->UnBind();
	ibo->UnBind();
	glBindVertexArray(0);
	textureSlotsIndex = 1;
	textureSlots[0] = whitetexture->m_RendererID;
	for (unsigned int i = 2; i < 32; i++)
	{
		textureSlots[i] = 0;
		glBindTextureUnit(i, 0);
	}
}
void Doom::Batch::flushLines(Shader * shader)
{
	glBindVertexArray(Lvao);
	Libo->Bind();
	shader->Bind();
	shader->UploadUnifromMat4("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
	glLineWidth(Line::width);
	glDrawElements(GL_LINES, Lindexcount, GL_UNSIGNED_INT, nullptr);
	glLineWidth(1.0f);
	Renderer::DrawCalls++;
	shader->UnBind();
	Libo->UnBind();
	glBindVertexArray(0);
}
void Batch::initText()
{
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, RENDERER_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(vao, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, vertex));

	glEnableVertexArrayAttrib(vao, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, textcoords));

	glEnableVertexArrayAttrib(vao, 2);
	glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, m_static));

	glEnableVertexArrayAttrib(vao, 3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, m_color));

	glEnableVertexArrayAttrib(vao, 4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, isGui));

	glEnableVertexArrayAttrib(vao, 5);
	glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, texIndex));

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

	ibo = new IndexBuffer(indices, RENDERER_INDICES_SIZE);
	glBindVertexArray(0);

	textureSlots[0] = whitetexture->m_RendererID;
	for (unsigned int i = 1; i < 32; i++)
	{
		textureSlots[i] = 0;
	}

}

void Batch::initGameObjects()
{
	glGenVertexArrays(1, &Gvao);
	glBindVertexArray(Gvao);

	glGenBuffers(1, &Gvbo);
	glBindBuffer(GL_ARRAY_BUFFER, Gvbo);
	glBufferData(GL_ARRAY_BUFFER, RENDERER_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(Gvao, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, vertex));

	glEnableVertexArrayAttrib(Gvao, 1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, textcoords));

	glEnableVertexArrayAttrib(Gvao, 2);
	glVertexAttribPointer(2, 1, GL_INT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, m_static));

	glEnableVertexArrayAttrib(Gvao, 3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, m_color));

	glEnableVertexArrayAttrib(Gvao, 4);
	glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, texIndex));

	glEnableVertexArrayAttrib(Gvao, 5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, rotationMat0));

	glEnableVertexArrayAttrib(Gvao, 6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, rotationMat1));

	glEnableVertexArrayAttrib(Gvao, 7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, rotationMat2));

	glEnableVertexArrayAttrib(Gvao, 8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, rotationMat3));

	glEnableVertexArrayAttrib(Gvao, 9);
	glVertexAttribPointer(9, 1, GL_INT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, m_particle));

	glEnableVertexArrayAttrib(Gvao, 10);
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, posMat0));

	glEnableVertexArrayAttrib(Gvao, 11);
	glVertexAttribPointer(11, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, posMat1));

	glEnableVertexArrayAttrib(Gvao, 12);
	glVertexAttribPointer(12, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, posMat2));

	glEnableVertexArrayAttrib(Gvao, 13);
	glVertexAttribPointer(13, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, posMat3));

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

	Gibo = new IndexBuffer(indices, RENDERER_INDICES_SIZE);
	glBindVertexArray(0);

	GtextureSlots[0] = whitetexture->m_RendererID;
	for (unsigned int i = 1; i < 32; i++)
	{
		GtextureSlots[i] = 0;
	}
}

void Doom::Batch::initLines()
{
	glGenVertexArrays(1, &Lvao);
	glBindVertexArray(Lvao);

	glGenBuffers(1, &Lvbo);
	glBindBuffer(GL_ARRAY_BUFFER, Lvbo);
	glBufferData(GL_ARRAY_BUFFER, RENDERER_BUFFER_SIZE, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexArrayAttrib(Lvao, 0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, m_vertex));

	glEnableVertexArrayAttrib(Lvao, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, m_color));

	glEnableVertexArrayAttrib(Lvao, 2);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, rotationMat0));

	glEnableVertexArrayAttrib(Lvao, 3);
	glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, rotationMat1));

	glEnableVertexArrayAttrib(Lvao, 4);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, rotationMat2));

	glEnableVertexArrayAttrib(Lvao, 5);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, rotationMat3));

	glEnableVertexArrayAttrib(Lvao, 6);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, MVPMat0));

	glEnableVertexArrayAttrib(Lvao, 7);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, MVPMat1));

	glEnableVertexArrayAttrib(Lvao, 8);
	glVertexAttribPointer(8, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, MVPMat2));

	glEnableVertexArrayAttrib(Lvao, 9);
	glVertexAttribPointer(9, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, MVPMat3));

	glEnableVertexArrayAttrib(Lvao, 10);
	glVertexAttribPointer(10, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, m_color));

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	unsigned int indices[RENDERER_INDICES_SIZE];

	unsigned int offset = 0;
	for (unsigned int i = 0; i < RENDERER_INDICES_SIZE; i += 2)
	{
		indices[i + 0] = offset + 0;
		indices[i + 1] = offset + 1;

		offset += 2;
	}

	Libo = new IndexBuffer(indices, RENDERER_INDICES_SIZE);
	glBindVertexArray(0);
}