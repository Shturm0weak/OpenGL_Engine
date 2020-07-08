#include "Collision.h"
#include "../Render/Renderer.h"
#include "../pch.h"


using namespace Doom;

Collision::Collision(GameObject* owner,double x, double y){
	SetType("Collision");
	this->owner = owner;
	id = Renderer::col_id;
	Renderer::col_id++;
	Renderer::collision2d.push_back(this);
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
	float theta = (-owner->GetAngle() * (2 * 3.14159f) / 360.0f);
	/*glm::vec2 p1 = glm::vec2(cos(theta) * (ScaledVerPos[0] + position.x)- sin(theta) * (ScaledVerPos[1] + position.y),
		sin(theta) * (ScaledVerPos[0] + position.x) + cos(theta) * (ScaledVerPos[1] + position.y));
	glm::vec2 p2 = glm::vec2(cos(theta) * (ScaledVerPos[4] + position.x) - sin(theta) * (ScaledVerPos[5] + position.y),
		sin(theta) * (ScaledVerPos[4] + position.x) + cos(theta) * (ScaledVerPos[5] + position.y));
	glm::vec2 p3 = glm::vec2(cos(theta) * (ScaledVerPos[8] + position.x) - sin(theta) * (ScaledVerPos[9] + position.y),
		sin(theta) * (ScaledVerPos[8] + position.x) + cos(theta) * (ScaledVerPos[9] + position.y));
	glm::vec2 p4 = glm::vec2(cos(theta) * (ScaledVerPos[12] + position.x) - sin(theta) * (ScaledVerPos[13] + position.y),
		sin(theta) * (ScaledVerPos[12] + position.x) + cos(theta) * (ScaledVerPos[13] + position.y));*/
	glm::vec2 p1 = glm::vec2(ScaledVerPos[0] + position.x, ScaledVerPos[1] + position.y);
	glm::vec2 p2 = glm::vec2(ScaledVerPos[4] + position.x, ScaledVerPos[5] + position.y);
	glm::vec2 p3 = glm::vec2(ScaledVerPos[8] + position.x, ScaledVerPos[9] + position.y);
	glm::vec2 p4 = glm::vec2(ScaledVerPos[12] + position.x, ScaledVerPos[13] + position.y);
	p.push_back(p1);
	p.push_back(p2);
	p.push_back(p3);
	p.push_back(p4);
	if (owner != nullptr) {
		Translate(owner->GetPositions().x, owner->GetPositions().y);
		SetOffset(offset.x, offset.y);
	}
}

Doom::Collision::~Collision()
{
	Renderer::collision2d.erase(GetId() + Renderer::collision2d.begin());
	Renderer::col_id--;
	unsigned int size = Renderer::collision2d.size();
	if (GetId() != size) {
		for (unsigned int i = GetId(); i < size; i++)
		{
			Renderer::collision2d[i]->SetId(i);
		}
	}
	owner->GetComponent<Transform>()->col = nullptr;
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
	this->position.x = x + offset.x;
	this->position.y = y + offset.y;	
	for (unsigned int i = 0; i < p.size(); i++) {
		p[i].x = ScaledVerPos[4 * i] + position.x;
		p[i].y = ScaledVerPos[4 * i + 1] + position.y;
	}
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
	this->pos = translate(glm::mat4(1.f), glm::vec3(position.x + offset.x, position.y+ offset.y, 0));
	UpdateCollision(position.x, position.y, pos, view, this->scale);
}

void Collision::SetOffset(float x, float y) {
	offset.x = x;
	offset.y = y;
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
			if (collision2d[i]->IsCollisionEnabled() == true) {
				if (this != &collision2d[i].get()) {
					col = dynamic_cast<Collision*>(collision2d[i]->GetCollisionReference());
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

void Collision::IsCollidedSAT() {
	isCollided = false;
	if (this == nullptr) {
		return;
	}
	if (Enable == true) {
		uint32_t sizeCol = Renderer::collision2d.size();
		for (unsigned int i = 0; i < sizeCol; i++)
		{
			if (Renderer::collision2d[i]->Enable == true) {
				col = Renderer::collision2d[i];
				if (this != col) {
					if (col == nullptr) {
						return;
					}
					if (col->IsTrigger) {
						if (ShapeOverlap_SAT(*this, *col)) {
							isCollided = true;
							collidedObject = col;
							std::function<void()> f2 = std::bind(&EventSystem::SendEvent,EventSystem::GetInstance(),"OnCollision", (Listener*)(owner), (void*)this->collidedObject);
							std::function<void()>* f1 = new std::function<void()>(f2);
							EventSystem::GetInstance()->SendEvent("OnMainThreadProcess", nullptr, f1);
							//EventSystem::Instance()->SendEvent("OnCollision", (Listener*)(owner), (void*)this->collidedObject);
						}
						else {
							isCollided = false;
							collidedObject = nullptr;
						}
					}
					else
						ShapeOverlap_SAT_STATIC(*this, *col);
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
		uint32_t sizeCol = Renderer::collision2d.size();
		for (unsigned int i = 0; i < sizeCol; i++)
		{
			if (Renderer::collision2d[i]->Enable == true) {
				col = Renderer::collision2d[i];
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

bool Collision::ShapeOverlap_SAT_STATIC(Collision &r1, Collision &r2)
{
	Collision *poly1 = &r1;
	Collision *poly2 = &r2;

	float overlap = INFINITY;

	for (int shape = 0; shape < 2; shape++)
	{
		if (shape == 1)
		{
			poly1 = &r2;
			poly2 = &r1;
		}

		for (int a = 0; a < poly1->p.size(); a++)
		{
			int b = (a + 1) % poly1->p.size();
			glm::vec2 axisProj = glm::vec2( -(poly1->p[b].y - poly1->p[a].y), poly1->p[b].x - poly1->p[a].x );

			// Optional normalisation of projection axis enhances stability slightly
			float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
			axisProj = { axisProj.x / d, axisProj.y / d };

			// Work out min and max 1D points for r1
			float min_r1 = INFINITY, max_r1 = -INFINITY;
			for (int p = 0; p < poly1->p.size(); p++)
			{
				float q = (poly1->p[p].x * axisProj.x + poly1->p[p].y * axisProj.y);
				min_r1 = std::fmin(min_r1, q);
				max_r1 = std::fmax(max_r1, q);
			}

			// Work out min and max 1D points for r2
			float min_r2 = INFINITY, max_r2 = -INFINITY;
			for (int p = 0; p < poly2->p.size(); p++)
			{
				float q = (poly2->p[p].x * axisProj.x + poly2->p[p].y * axisProj.y);
				min_r2 = std::fmin(min_r2, q);
				max_r2 = std::fmax(max_r2, q);
			}

			// Calculate actual overlap along projected axis, and store the minimum
			overlap = std::fmin(std::fmin(max_r1, max_r2) - std::fmax(min_r1, min_r2), overlap);

			if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
				return false;
		}
	}

	// If we got here, the objects have collided, we will displace r1
	// by overlap along the vector between the two object centers
	Transform* trans1 = r1.owner->component_manager->GetComponent<Transform>();
	Transform* trans2 = r2.owner->component_manager->GetComponent<Transform>();
	glm::vec2 d = glm::vec2(trans2->position.x - trans1->position.x, trans2->position.y - trans1->position.y);
	double s = sqrtf(d.x*d.x + d.y*d.y);

	//If displacement.x is set then even with 0 angle 	
	//of an collided object will move our player along the x axis
	//only if the both objects have not the same x
	//so it needs to be overthought either do we need continues collision detection or not
	glm::vec2 posToTranslate = glm::vec2(0,0);
	
	posToTranslate.x = trans1->position.x;
	if ((r2.positions[9] * r2.owner->scaleValues[1]) + r2.owner->GetPositions().y - 0.15 > (r1.positions[1] * r1.owner->scaleValues[1]) + r1.owner->GetPositions().y)
		posToTranslate.x = trans1->position.x - (overlap * d.x / s);
		
	posToTranslate.y = trans1->position.y - (overlap * d.y / s);
	trans1->Translate(posToTranslate.x, posToTranslate.y);
	std::function<void()> f2 = std::bind(&EventSystem::SendEvent, EventSystem::GetInstance(), "OnCollision", (Listener*)(owner), &r2);
	std::function<void()>* f1 = new std::function<void()>(f2);
	EventSystem::GetInstance()->SendEvent("OnMainThreadProcess", nullptr, f1);
	return false;
}

bool Collision::ShapeOverlap_SAT(Collision &r1, Collision &r2)
{
	Collision *poly1 = &r1;
	Collision *poly2 = &r2;
	
	float overlap = INFINITY;

	for (int shape = 0; shape < 2; shape++)
	{
		if (shape == 1)
		{
			poly1 = &r2;
			poly2 = &r1;
		}

		for (int a = 0; a < poly1->p.size(); a++)
		{
			int b = (a + 1) % poly1->p.size();
			glm::vec2 axisProj = glm::vec2(-(poly1->p[b].y - poly1->p[a].y), poly1->p[b].x - poly1->p[a].x);

			// Optional normalisation of projection axis enhances stability slightly
			//float d = sqrtf(axisProj.x * axisProj.x + axisProj.y * axisProj.y);
			//axisProj = { axisProj.x / d, axisProj.y / d };

			// Work out min and max 1D points for r1
			float min_r1 = INFINITY, max_r1 = -INFINITY;
			for (int p = 0; p < poly1->p.size(); p++)
			{
				float q = (poly1->p[p].x * axisProj.x + poly1->p[p].y * axisProj.y);
				min_r1 = std::fmin(min_r1, q);
				max_r1 = std::fmax(max_r1, q);
			}

			// Work out min and max 1D points for r2
			float min_r2 = INFINITY, max_r2 = -INFINITY;
			for (int p = 0; p < poly2->p.size(); p++)
			{
				float q = (poly2->p[p].x * axisProj.x + poly2->p[p].y * axisProj.y);
				min_r2 = std::fmin(min_r2, q);
				max_r2 = std::fmax(max_r2, q);
			}

			// Calculate actual overlap along projected axis, and store the minimum
			overlap = std::fmin(std::fmin(max_r1, max_r2) - std::fmax(min_r1, min_r2), overlap);

			if (!(max_r2 >= min_r1 && max_r1 >= min_r2))
				return false;
		}
	}
	return true;
}