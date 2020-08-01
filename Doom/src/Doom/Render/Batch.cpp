#include "Batch.h"
#include "../Core/Timer.h"
#include "Gui.h"

using namespace Doom;

Batch::Batch()
{
	std::cout << BOLDGREEN << "Initialized Batch rendering" << RESET << std::endl;
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

void Batch::Submit(Character* c)
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
		if (textureSlots[i] == c->texture->m_RendererID) {
			textureIndex = (float)i;
			break;
		}
	}

	if (textureIndex == 0.0f) {
		textureIndex = (float)textureSlotsIndex;
		textureSlots[textureSlotsIndex] = c->texture->m_RendererID;
		textureSlotsIndex++;
	}

	buffer->vertex = glm::vec2((c->mesh2D[0] * c->_scale.x + c->position.x), ((c->mesh2D[1]) * c->_scale.y + c->position.y));
	buffer->textcoords = glm::vec2(c->mesh2D[2], c->mesh2D[3]);
	buffer->m_static = c->isRelatedToCam;
	buffer->m_color = c->color;
	buffer->isGui = 0.;
	buffer->texIndex = textureIndex;
	buffer++;

	buffer->vertex = glm::vec2((c->mesh2D[4] * c->_scale.x + c->position.x), ((c->mesh2D[5]) * c->_scale.y + c->position.y));
	buffer->textcoords = glm::vec2(c->mesh2D[6], c->mesh2D[7]);
	buffer->m_static = c->isRelatedToCam;
	buffer->m_color = c->color;
	buffer->isGui = 0.;
	buffer->texIndex = textureIndex;
	buffer++;

	buffer->vertex = glm::vec2((c->mesh2D[8] * c->_scale.x + c->position.x), ((c->mesh2D[9])* c->_scale.y + c->position.y));
	buffer->textcoords = glm::vec2(c->mesh2D[10], c->mesh2D[11]);
	buffer->m_static = c->isRelatedToCam;
	buffer->m_color = c->color;
	buffer->isGui = 0.;
	buffer->texIndex = textureIndex;
	buffer++;

	buffer->vertex = glm::vec2((c->mesh2D[12] * c->_scale.x + c->position.x), ((c->mesh2D[13]) * c->_scale.y + c->position.y));
	buffer->textcoords = glm::vec2(c->mesh2D[14], c->mesh2D[15]);
	buffer->m_static = c->isRelatedToCam;
	buffer->m_color = c->color;
	buffer->isGui = 0.;
	buffer->texIndex = textureIndex;
	buffer++;

	indexcount += 6;
}

void Doom::Batch::Submit(glm::mat4 pos, glm::mat4 view, glm::vec4 color, glm::vec2 scale, Texture * texture)
{

	float mesh2D[16] = {
		-0.5f, -0.5f, 0.0f, 0.0f,
		 0.5f, -0.5f, 1.0f, 0.0f,
		 0.5f,  0.5f, 1.0f, 1.0f,
		-0.5f,  0.5f, 0.0f, 1.0f
	};

	if (GtextureSlotsIndex > maxTextureSlots - 1 || Gindexcount >= RENDERER_INDICES_SIZE) {
		Batch::GetInstance()->EndGameObjects();
		Batch::GetInstance()->flushGameObjects(Batch::GetInstance()->BasicShader);
		Batch::GetInstance()->BeginGameObjects();
	}
	GtextureIndex = 0.0f;

	if (texture != nullptr) {
		for (unsigned int i = 0; i < maxTextureSlots; i++)
		{
			if (GtextureSlots[i] == texture->m_RendererID) {
				GtextureIndex = (float)i;
				break;
			}

		}
		if (GtextureIndex == 0.0f) {
			GtextureIndex = (float)GtextureSlotsIndex;
			GtextureSlots[GtextureSlotsIndex] = texture->m_RendererID;
			GtextureSlotsIndex++;
		}
	}

	Gbuffer->vertex = glm::vec2(mesh2D[0] * scale.x, mesh2D[1] * scale.y);
	Gbuffer->textcoords = glm::vec2(mesh2D[2], mesh2D[3]);
	Gbuffer->m_static = false;
	Gbuffer->m_color = color;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = view[0];
	Gbuffer->rotationMat1 = view[1];
	Gbuffer->rotationMat2 = view[2];
	Gbuffer->rotationMat3 = view[3];
	Gbuffer->posMat0 = pos[0];
	Gbuffer->posMat1 = pos[1];
	Gbuffer->posMat2 = pos[2];
	Gbuffer->posMat3 = pos[3];
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(mesh2D[4] * scale.x, mesh2D[5] * scale.y);
	Gbuffer->textcoords = glm::vec2(mesh2D[6], mesh2D[7]);
	Gbuffer->m_static = false;
	Gbuffer->m_color = color;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = view[0];
	Gbuffer->rotationMat1 = view[1];
	Gbuffer->rotationMat2 = view[2];
	Gbuffer->rotationMat3 = view[3];
	Gbuffer->posMat0 = pos[0];
	Gbuffer->posMat1 = pos[1];
	Gbuffer->posMat2 = pos[2];
	Gbuffer->posMat3 = pos[3];
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(mesh2D[8] * scale.x, mesh2D[9] * scale.y);
	Gbuffer->textcoords = glm::vec2(mesh2D[10], mesh2D[11]);
	Gbuffer->m_static = false;
	Gbuffer->m_color = color;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = view[0];
	Gbuffer->rotationMat1 = view[1];
	Gbuffer->rotationMat2 = view[2];
	Gbuffer->rotationMat3 = view[3];
	Gbuffer->posMat0 = pos[0];
	Gbuffer->posMat1 = pos[1];
	Gbuffer->posMat2 = pos[2];
	Gbuffer->posMat3 = pos[3];
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(mesh2D[12] * scale.x, mesh2D[13] * scale.y);
	Gbuffer->textcoords = glm::vec2(mesh2D[14], mesh2D[15]);
	Gbuffer->m_static = false;
	Gbuffer->m_color = color;
	Gbuffer->texIndex = GtextureIndex;
	Gbuffer->rotationMat0 = view[0];
	Gbuffer->rotationMat1 = view[1];
	Gbuffer->rotationMat2 = view[2];
	Gbuffer->rotationMat3 = view[3];
	Gbuffer->posMat0 = pos[0];
	Gbuffer->posMat1 = pos[1];
	Gbuffer->posMat2 = pos[2];
	Gbuffer->posMat3 = pos[3];
	Gbuffer++;

	Gindexcount += 6;
}

void Batch::Submit(float* mesh2D,glm::vec4 color,Texture* texture, glm::vec2 size, glm::vec2 pos,float radius) {

	if (textureSlotsIndex > maxTextureSlots - 1) {
		End();
		flushText(shader);
		Begin();
	}

	textureIndex = 0.0f;

	if (texture != nullptr) {
		for (unsigned int i = 0; i < maxTextureSlots; i++)
		{
			if (textureSlots[i] == texture->m_RendererID) {
				textureIndex = (float)i;
				break;
			}

		}
		if (textureIndex == 0.0f) {
			textureIndex = (float)textureSlotsIndex;
			textureSlots[textureSlotsIndex] = texture->m_RendererID;
			textureSlotsIndex++;
		}
	}

	buffer->vertex = glm::vec2((mesh2D[0]), mesh2D[1]);
	buffer->textcoords = glm::vec2(0,0);
	buffer->m_static = true;
	buffer->m_color = color;
	buffer->isGui = 1.;
	buffer->texIndex = textureIndex;
	buffer->size = size;
	buffer->pos = pos;
	buffer->raduis = radius;
	buffer->viewportSize = glm::vec2(Window::GetSize()[0], Window::GetSize()[1]);
	buffer++;

	buffer->vertex = glm::vec2((mesh2D[2]), (mesh2D[3]));
	buffer->textcoords = glm::vec2(1.0f, 0.0f);
	buffer->m_static = true;
	buffer->m_color = color;
	buffer->isGui = 1.;
	buffer->texIndex = textureIndex;
	buffer->size = size;
	buffer->pos = pos;
	buffer->raduis = radius;
	buffer->viewportSize = glm::vec2(Window::GetSize()[0], Window::GetSize()[1]);
	buffer++;

	buffer->vertex = glm::vec2((mesh2D[4]), (mesh2D[5]));
	buffer->textcoords = glm::vec2(1.0f, 1.0f);
	buffer->m_static = true;
	buffer->m_color = color;
	buffer->isGui = 1.;
	buffer->texIndex = textureIndex;
	buffer->size = size;
	buffer->pos = pos;
	buffer->raduis = radius;
	buffer->viewportSize = glm::vec2(Window::GetSize()[0], Window::GetSize()[1]);
	buffer++;

	buffer->vertex = glm::vec2((mesh2D[6]), ((mesh2D[7])));
	buffer->textcoords = glm::vec2(0.0f, 1.0f);
	buffer->m_static = true;
	buffer->m_color = color;
	buffer->isGui = 1.;
	buffer->texIndex = textureIndex;
	buffer->size = size;
	buffer->pos = pos;
	buffer->raduis = radius;
	buffer->viewportSize = glm::vec2(Window::GetSize()[0], Window::GetSize()[1]);
	buffer++;

	indexcount += 6;
}

#include "../Core/Editor.h"

void Doom::Batch::Submit(Line & line)
{
	if (Lindexcount >= RENDERER_MAX_SPRITES) {
		EndLines();
		flushLines(LineShader);
		BeginLines();
	}

	Lbuffer->m_vertex = glm::vec3(line.mesh2D[0], line.mesh2D[1], line.mesh2D[2]);
	Lbuffer->m_color = line.color;

	Lbuffer++;

	Lbuffer->m_vertex = glm::vec3(line.mesh2D[3], line.mesh2D[4],line.mesh2D[5]);
	Lbuffer->m_color = line.color;

	Lbuffer++;

	Lindexcount += 2;
}


void Doom::Batch::Submit(glm::vec4 color, float* mesh2D)
{

	if (Lindexcount >= RENDERER_INDICES_SIZE) {
		EndLines();
		flushLines(LineShader);
		BeginLines();
		Lindexcount = 0;
	}

	Lbuffer->m_vertex = glm::vec3(mesh2D[0], mesh2D[1], mesh2D[2]);
	Lbuffer->m_color = color;

	Lbuffer++;

	Lbuffer->m_vertex = glm::vec3(mesh2D[3], mesh2D[4], mesh2D[5]);
	Lbuffer->m_color = color;

	Lbuffer++;

	Lindexcount += 3;
}

void Batch::Submit(SpriteRenderer & c)
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

	Gbuffer->vertex = glm::vec2(c.mesh2D[0] * c.owner->scaleValues[0], c.mesh2D[1] * c.owner->scaleValues[1]);
	Gbuffer->textcoords = glm::vec2(c.mesh2D[2], c.mesh2D[3]);
	Gbuffer->m_static = c.owner->Static;
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
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(c.mesh2D[4] * c.owner->scaleValues[0], c.mesh2D[5] * c.owner->scaleValues[1]);
	Gbuffer->textcoords = glm::vec2(c.mesh2D[6], c.mesh2D[7]);
	Gbuffer->m_static = c.owner->Static;
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
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(c.mesh2D[8] * c.owner->scaleValues[0], c.mesh2D[9] * c.owner->scaleValues[1]);
	Gbuffer->textcoords = glm::vec2(c.mesh2D[10], c.mesh2D[11]);
	Gbuffer->m_static = c.owner->Static;
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
	Gbuffer++;

	Gbuffer->vertex = glm::vec2(c.mesh2D[12] * c.owner->scaleValues[0], c.mesh2D[13] * c.owner->scaleValues[1]);
	Gbuffer->textcoords = glm::vec2(c.mesh2D[14], c.mesh2D[15]);
	Gbuffer->m_static = c.owner->Static;
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
	Gbuffer++;

	Gindexcount += 6;
}

void Batch::Submit(RectangleCollider2D& c) {
	glm::mat4 mvp = c.owner->GetComponentManager()->GetComponent<Irenderer>()->pos * c.owner->GetComponentManager()->GetComponent<Irenderer>()->view;
	if (GtextureSlotsIndex > maxTextureSlots - 1) {
		EndGameObjects();
		flushGameObjects(shader);
		BeginGameObjects();
		std::cout << "max texture" << std::endl;
	}

	GtextureIndex = 0.0f;
	//std::cout << "Batch " << c.position.x << "	" << c.position.y << std::endl;
	glm::vec3 scaleVal = c.owner->GetScale();
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
	shader->SetUniformMat4f("u_Projection", Gui::GetInstance()->ViewProjecTionRelatedToCamera);
	shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());

	glDrawElements(GL_TRIANGLES, Gindexcount, GL_UNSIGNED_INT, NULL);
	Renderer::DrawCalls++;
	shader->UnBind();
	Gibo->UnBind();
	GtextureSlotsIndex = 2;
	Gindexcount = 0;
	for (unsigned int i = GtextureSlotsIndex; i < 32; i++)
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
	shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
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
	buffer = bufferPtr;
	indexcount = 0;
}

void Batch::End() {
	uint32_t dataSize = (uint8_t*)buffer - (uint8_t*)bufferPtr;
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, bufferPtr);
}

void Batch::BeginGameObjects()
{
	Gbuffer = GbufferPtr;
	Gindexcount = 0;
}

void Batch::EndGameObjects()
{
	uint32_t dataSize = (uint8_t*)Gbuffer - (uint8_t*)GbufferPtr;
	glBindBuffer(GL_ARRAY_BUFFER, Gvbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, GbufferPtr);
}

void Doom::Batch::BeginLines()
{
	Lbuffer = LbufferPtr;
	Lindexcount = 0;
}

void Doom::Batch::EndLines()
{
	uint32_t dataSize = (uint8_t*)Lbuffer - (uint8_t*)LbufferPtr;
	glBindBuffer(GL_ARRAY_BUFFER, Lvbo);
	glBufferSubData(GL_ARRAY_BUFFER, 0, dataSize, LbufferPtr);
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
	shader->SetUniformMat4f("u_Projection", Gui::GetInstance()->ViewProjecTionRelatedToCamera);
	shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
	shader->SetUniform2fv("u_offset", Gui::GetInstance()->textProps.shadowOffset);
	shader->SetUniform1f("u_width", Gui::GetInstance()->textProps.width);
	shader->SetUniform1f("u_edge", Gui::GetInstance()->textProps.edge);
	shader->SetUniform1f("u_borderwidth", Gui::GetInstance()->textProps.borderwidth);
	shader->SetUniform1f("u_borderedge", Gui::GetInstance()->textProps.borderedge);
	shader->SetUniform4fv("u_outlineColor", Gui::GetInstance()->textProps.outLineColor);

	glDrawElements(GL_TRIANGLES, indexcount, GL_UNSIGNED_INT, NULL);
	Renderer::DrawCalls++;
	shader->UnBind();
	ibo->UnBind();
	glBindVertexArray(0);
	textureSlotsIndex = 1;
	textureSlots[0] = whitetexture->m_RendererID;
	for (unsigned int i = textureSlotsIndex; i < 32; i++)
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
	shader->SetUniformMat4f("u_ViewProjection", Window::GetCamera().GetViewProjectionMatrix());
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

	glEnableVertexArrayAttrib(vao, 6);
	glVertexAttribPointer(6, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, size));

	glEnableVertexArrayAttrib(vao, 7);
	glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, pos));

	glEnableVertexArrayAttrib(vao, 8);
	glVertexAttribPointer(8, 1, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, raduis));

	glEnableVertexArrayAttrib(vao, 9);
	glVertexAttribPointer(9, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)offsetof(Vertex, viewportSize));

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

	bufferPtr = new Vertex[RENDERER_MAX_SPRITES];
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

	GbufferPtr = new Vertex[RENDERER_MAX_SPRITES];
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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, m_vertex));

	glEnableVertexArrayAttrib(Lvao, 1);
	glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(VertexLine), (const GLvoid*)offsetof(VertexLine, m_color));

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
	LbufferPtr = new VertexLine[RENDERER_MAX_SPRITES];
	glBindVertexArray(0);
}