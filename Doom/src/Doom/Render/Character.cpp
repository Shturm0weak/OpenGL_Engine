#include "Character.h"

using namespace Doom;

Character::~Character()
{
	delete[] indeces2D;
	delete[] mesh2D;
}

void Character::Changecharater(Font* font,char ch)
{
	{
		int index = (int)ch;
		int i = 0;
		while (index != font->id[i])
			i++;
		id = font->id[i];
		x = font->x[i];
		y = font->y[i];
		width = font->width[i];
		height = font->height[i];
		xoffset = font->xoffset[i];
		yoffset = font->yoffset[i];
		xadvance = font->xadvance[i];
	}
	{
		mesh2D[2] = (x) / 512.;
		mesh2D[3] = (abs(y - 512) - height) / 512.;
		mesh2D[6] = (x + width) / 512.;
		mesh2D[7] = (abs(y - 512) - height) / 512.;
		mesh2D[10] = (x + width) / 512.;
		mesh2D[11] = abs(y - 512) / 512.;
		mesh2D[14] = x / 512.;
		mesh2D[15] = abs(y - 512) / 512.;
		mesh2D[4] = (width / 2.) / 52.;
		mesh2D[0] = -(width / 2.) / 52.;
		mesh2D[8] = (width / 2.) / 52.;
		mesh2D[12] = -(width / 2.) / 52.;
		mesh2D[9] = (height / 2.) / 63.;
		mesh2D[13] = (height / 2.) / 63.;
		mesh2D[1] = -(height / 2.) / 63.;
		mesh2D[5] = -(height / 2.) / 63.;
	}
}

Character::Character(Font* font,char ch,float posx,float posy,float scale)
{
	shader = font->shader;
	this->font = font;
	position.x = posx;
	position.y = posy;
	texture = font->fontAtlas;
	this->ch = ch;
	{
		int index = (int)ch;
		int i = 0;
		while (index != font->id[i])
			i++;
		id = font->id[i];
		x = font->x[i];
		y = font->y[i];
		width = font->width[i];
		height = font->height[i];
		xoffset = font->xoffset[i];
		yoffset = font->yoffset[i];
		xadvance = font->xadvance[i];
	}
	/*if (ch == 'g' || ch == 'j' || ch == 'p' || ch == 'y' || ch == 'q')
		yoffset *= 1.5;*/
	{
		double size = font->fontAtlas->GetWidth();

		mesh2D[2] = (x) / size;
		mesh2D[3] = (abs(y - size) - height) / size;
		mesh2D[6] = (x + width) / size;
		mesh2D[7] = (abs(y - size) - height) / size;
		mesh2D[10] = (x + width) / size;
		mesh2D[11] = abs(y - size) / size;
		mesh2D[14] = x / size;
		mesh2D[15] = abs(y - size) / size;

		double widthRatio = Window::GetCamera().GetAspectRation().x / Window::GetProps()[0];
		double heightRatio = Window::GetCamera().GetAspectRation().y / Window::GetProps()[1];

		mesh2D[0]  = 0;
		mesh2D[1]  = 0;
		mesh2D[4]  = (widthRatio  * (width  + xoffset));
		mesh2D[5]  = 0;
		mesh2D[8]  = (widthRatio  * (width  + xoffset));
		mesh2D[9]  = (heightRatio * (height + yoffset));
		mesh2D[12] = 0;
		mesh2D[13] = (heightRatio * (height + yoffset));

		//std::cout << id << "	"<< ch <<"	"<< height << "	" << yoffset<<  std::endl;
		/*std::cout << "\n" << mesh2D[2] << "	" << mesh2D[3] << std::endl;
		std::cout << mesh2D[6] << "	" << mesh2D[7] << std::endl;
		std::cout << mesh2D[10] << "	" << mesh2D[11] << std::endl;
		std::cout << mesh2D[14] << "	" << mesh2D[15] << std::endl;*/
	}
	//this->pos = translate(glm::mat4(1.f), glm::vec3(position.x, position.y, 0));
	this->Scale(scale);
	InitShader();
	this->shader->UnBind();
	glm::vec2 proj = Window::GetCamera().GetAspectRation();
	ViewProjecTionRelatedToScreen = glm::ortho(-proj.x, proj.x, -proj.y, proj.y, 1.f, -1.f);
}

//void Character::OnRunning(OrthographicCamera& camera) {
//	
//	this->shader->Bind();
//	if (!isRelatedToCam) {
//		this->MVP = camera.GetProjectionMatrix() * pos * viewXscale;
//		this->shader->UploadUnifromMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
//	}
//	else {
//		this->MVP = ViewProjecTionRelatedToScreen * pos * viewXscale;
//		this->shader->UploadUnifromMat4("u_ViewProjection", ViewProjecTionRelatedToScreen);
//	}
//	this->viewXscale = scale * view;
//	this->shader->SetUniformMat4f("u_MVP", this->MVP);
//	this->texture->Bind(this->texture->m_RendererID);
//	Draw(*this->va, *this->ib, *this->shader);
//}


void Character::InitShader() {
	if (texture == (void*)0xCCCCCCCC || texture == nullptr) {
		std::cout << "InitShader" << " : " << "Texture doesn't exist!" << std::endl;
	}
	shader->Bind();
	this->texture->Bind(this->texture->m_RendererID);
	this->shader->SetUniform1i("u_Texture", this->texture->m_RendererID);
}

void Character::Scale(float argscale)
{
	yoffset *= argscale;
	_scale.x = argscale;
	_scale.y = argscale;
}

void Character::Translate(float x, float y)
{
	position.x = x;
	position.y = y;
}

Character * Character::operator=(Character * ch)
{
	Changecharater(ch->font, ch->ch);
	this->Scale(ch->_scale.y);
	return this;
}
