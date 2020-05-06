#include "../pch.h"
#include "Collision.h"

using namespace Doom;

Collision::Collision(GameObject* owner,double x, double y){
	SetType("Collision");
	this->owner = owner;
	id = Renderer2DLayer::col_id;
	Renderer2DLayer::col_id++;
	Renderer2DLayer::PushCol(*this);
	if (owner != nullptr) {
		position.x = owner->component_manager->GetComponent<Transform>()->position.x;
		position.y = owner->component_manager->GetComponent<Transform>()->position.y;
	}
	else {
		position.x = x;
		position.y = y;
	}
	this->pos = translate(glm::mat4(1.f), glm::vec3(position.x, position.y, 0));
	RealVerPos();
	p.push_back(glm::vec2(ScaledVerPos[0] + position.x, ScaledVerPos[1] + position.y));
	p.push_back(glm::vec2(ScaledVerPos[4] + position.x, ScaledVerPos[5] + position.y));
	p.push_back(glm::vec2(ScaledVerPos[8] + position.x, ScaledVerPos[9] + position.y));
	p.push_back(glm::vec2(ScaledVerPos[12] + position.x, ScaledVerPos[13] + position.y));
	glGenVertexArrays(1, &this->vao);
	glBindVertexArray(this->vao);
	this->layout->Push<float>(2);
	this->layout->Push<float>(2);
	this->va->AddBuffer(*this->vb, *this->layout);
	//shader->Bind();
	//shader->SetUniform4fv("U_Color", this->Color);
	this->va->UnBind();
	this->shader->UnBind();
	this->vb->UnBind();
	this->ib->UnBind();
	if (owner != nullptr) {
		Translate(owner->GetPositions().x, owner->GetPositions().y);
		SetOffset(offsetX, offsetY);
	}
}

void Collision::UpdateCollision(double x, double y,glm::mat4 pos,glm::mat4 view,glm::mat4 scale)
{
	this->scale = scale;
	this->view = view;
	if (scaleXview != view * this->scale) {
		this->scaleXview = view * this->scale;
		RealVerPos();
	}	
	this->pos = pos;
	this->position.x = x + offsetX;
	this->position.y = y + offsetY;	
	for (unsigned int i = 0; i < p.size(); i++) {
		p[i].x = ScaledVerPos[4 * i] + position.x;
		p[i].y = ScaledVerPos[4 * i + 1] + position.y;
	}
}

void Collision::OnRunning(OrthographicCamera& camera)
{
		this->shader->Bind();
		//this->pos = translate(glm::mat4(1.f), glm::vec3(position.x, position.y, 0));
		//this->scaleXview = scale * view;
		this->MVP = camera.GetProjectionMatrix() * pos * scaleXview;
		//camera.RecalculateViewMatrix();
		this->shader->UploadUnifromMat4("u_ViewProjection", camera.GetViewProjectionMatrix());
		this->shader->SetUniformMat4f("u_MVP", this->MVP);
	Renderer2DLayer::Draw(*this->va, *this->ib, *this->shader);
}

void Collision::RealVerPos() {
	float* pSource;
	pSource = (float*)glm::value_ptr(this->scaleXview);
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < 4; j++) {
			ScaledVerPos[i * 4 + j] = 0;
			for (unsigned int k = 0; k < 4; k++) {
				ScaledVerPos[i * 4 + j] += positions[i * 4 + k] * pSource[k * 4 + j];
			}
		}
	}
	pSource = nullptr;
}

float* Collision::GetVertexPositions() {
	return ScaledVerPos;
}

glm::vec2 Collision::NormalVector(glm::vec2 vec2)
{
	return glm::vec2(1,1 * (vec2.x * 1) / vec2.y);
}

void Collision::Scale(float x,float y) {
	if (this == nullptr) 
		return;
	scaleValues[0] = x;
	scaleValues[1] = y;
	this->scale = glm::scale(glm::mat4(1.f), glm::vec3(x, y, 0));
	RealVerPos();
	UpdateCollision(position.x, position.y, pos, view, this->scale);
}

void Collision::Translate(float x, float y)
{
	position.x = x;
	position.y = y;
	this->pos = translate(glm::mat4(1.f), glm::vec3(position.x + offsetX, position.y+ offsetY, 0));
	UpdateCollision(position.x, position.y, pos, view, this->scale);
}

void Collision::SetOffset(float x, float y) {
	offsetX = x;
	offsetY = y;
}

/*bool Collision::IsCollided() {
	std::unique_lock<std::mutex> lock(mtx);
	_isCollided = false;
	if (this == nullptr) {
		return false;
		
	}
	if (Enable == true) {
		Collided_side_right = false;
		Collided_side_left = false;
		Collided_side_top = false;
		Collided_side_bottom = false;
		for (unsigned int i = 0; i < collision2d.size(); i++)
		{
			if (collision2d[i].get().IsCollisionEnabled() == true) {
				if (this != &collision2d[i].get()) {
					col = dynamic_cast<Collision*>(collision2d[i].get().GetCollisionReference());
					if (col == nullptr) {
						return false;
						
					}

					arrver1 = col->GetVertexPositions();
					arrpos1 = col->GetPositions();
				
					if (GetDistance(position.x, position.y, arrpos1[0], arrpos1[1] < 1.1 * (arrver1[4] + ScaledVerPos[4]))) {
						right = ScaledVerPos[8] + position.x;
						left = ScaledVerPos[0] + position.x;
						top = ScaledVerPos[13] + position.y;
						bottom = ScaledVerPos[1] + position.y;
						col->right = arrver1[8] + arrpos1[0];
						col->left = arrver1[0] + arrpos1[0];
						col->top = arrver1[13] + arrpos1[1];
						col->bottom = arrver1[1] + arrpos1[1];
						difftb = abs(top) - abs(col->bottom);
						diffbt = abs(bottom) - abs(col->top);
						diffrl = abs(right) - abs(col->left);
						difflr = abs(left) - abs(col->right);
						if (right > col->left &&
							left < col->right &&
							bottom < col->top &&
							top > col->bottom) {

							if (diffbt < 0.3 && diffbt > -0.3) {
								Collided_side_bottom = true;
							}
							else if (difftb < 0.3 && difftb > -0.3) {
								Collided_side_top = true;
							}
							else if (diffrl < 0.3 && diffrl > -0.3) {
								Collided_side_right = true;
							}
							else if (difflr < 0.3 && difflr > -0.3) {
								Collided_side_left = true;
							}
							_collidedObj = col;
						}
						delete[] arrpos1;
						arrpos1 = nullptr;
						col = nullptr;
					}
				}
			}
		}
		if (Collided_side_bottom || Collided_side_left || Collided_side_right || Collided_side_top) {
			_isCollided = true;
			return true;
		}
		else
			return false;
	}
	else {
		return false;
	}
}*/

bool Collision::IsCollidedSAT() {
	isCollided = false;
	int check = 0;
	if (this == nullptr) {
		return false;
	}
	if (Enable == true) {
		for (unsigned int i = 0; i < collision2d.size(); i++)
		{
			if (collision2d[i].get().IsCollisionEnabled() == true) {
				col = dynamic_cast<Collision*>(collision2d[i].get().GetCollisionReference());
				if (this != col) {
					if (col == nullptr) {
						return false;
					}

					for (unsigned int a = 0; a < p.size(); a++)
					{
						int b = (a + 1) % p.size();
						glm::vec2 axisProj = { -(p[b].x - p[a].x), p[b].y - p[a].y };
						float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
						axisProj = { axisProj.y / d, axisProj.x / d };

						float min_r1 = INFINITY, max_r1 = -INFINITY;

						for (unsigned int p = 0; p < this->p.size(); p++)
						{
							float q = (this->p[p].x * axisProj.x + this->p[p].y * axisProj.y);
							min_r1 = std::fmin(min_r1, q);
							max_r1 = std::fmax(max_r1, q);
						}

						float min_r2 = INFINITY, max_r2 = -INFINITY;
						for (unsigned int p = 0; p < col->p.size(); p++)
						{
							float q = (col->p[p].x * axisProj.x + col->p[p].y * axisProj.y);
							min_r2 = std::fmin(min_r2, q);
							max_r2 = std::fmax(max_r2, q);
						}

						if (!(max_r2 >= min_r1 && max_r1 >= min_r2)) {
							check++;	
							collidedObject = nullptr;
						}

					}
					if (check > 0) {
						check = 0;
						continue;
					}
					collidedObject = col;
					return true;
					
				}
			}
			
		}

	}
}

void Collision::IsCollidedDIAGS()
{
	std::lock_guard<std::mutex> lock(mtx);
	isCollided = false;
	collidedObject = nullptr;
	if (this == nullptr) {
		return;
	}
	if (Enable == true) {

		for (unsigned int i = 0; i < collision2d.size(); i++)
		{
			if (collision2d[i].get().IsCollisionEnabled() == true) {
				col = dynamic_cast<Collision*>(collision2d[i].get().GetCollisionReference());
				if (this != col) {
					if (col == nullptr) {
						return;
					}
					
					for (unsigned int p = 0; p < this->p.size(); p++)
					{
						glm::vec2 line_r1s = { this->position.x, this->position.y };
						glm::vec2 line_r1e = this->p[p];
						displacement.x = 0;
						displacement.y = 0;
						// ...against edges of the other
						for (unsigned int q = 0; q < col->p.size(); q++)
						{
							glm::vec2 line_r2s = col->p[q];
							glm::vec2 line_r2e = col->p[(q + 1) % col->p.size()];

							// Standard "off the shelf" line segment intersection
							float h = (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r1e.y) - (line_r1s.x - line_r1e.x) * (line_r2e.y - line_r2s.y);
							float t1 = ((line_r2s.y - line_r2e.y) * (line_r1s.x - line_r2s.x) + (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r2s.y)) / h;
							float t2 = ((line_r1s.y - line_r1e.y) * (line_r1s.x - line_r2s.x) + (line_r1e.x - line_r1s.x) * (line_r1s.y - line_r2s.y)) / h;

							if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
							{
								if (col->IsTrigger == false) {
									displacement.x += (1.f - t1) * (line_r1e.x - line_r1s.x);
									displacement.y += (1.f - t1) * (line_r1e.y - line_r1s.y);
								}
								collidedObject = col;
								isCollided = true;
							}
						}
						if (IsTrigger == false) {
							trans = owner->component_manager->GetComponent<Transform>();
							trans->position.x -= displacement.x;
							trans->position.y -= displacement.y;
							trans = nullptr;
						}
						
					}
				}
			}
		}
		return;
	}
}