#include "../pch.h"
#include "RectangleCollider2D.h"
#include "../Render/Renderer.h"
#include "Core/World.h"

using namespace Doom;

void Doom::RectangleCollider2D::Delete()
{
	s_FreeMemory.push_back(m_MemoryPoolPtr);
	auto iter = std::find(s_Collision2d.begin(), s_Collision2d.end(), this);
	if (iter != s_Collision2d.end())
		s_Collision2d.erase(iter);
}

Component* Doom::RectangleCollider2D::Create()
{
	char* ptr = Utils::PreAllocateMemory<RectangleCollider2D>(s_MemoryPool, s_FreeMemory);
	RectangleCollider2D* component = (RectangleCollider2D*)((void*)ptr); //= new(iter->first + iter->second * sizeof(RectangleCollider2D)) RectangleCollider2D();
	component->m_MemoryPoolPtr = ptr;
	RectangleCollider2D::s_Collision2d.push_back(component);
	RectangleCollider2D::s_CollidersToInit.push_back(component);
	return component;
}

RectangleCollider2D::RectangleCollider2D(GameObject* owner,double x, double y)
{
	this->m_OwnerOfCom = owner;
	World::GetInstance().s_ColId++;
	SetOffset(m_Offset.x, m_Offset.y);
	s_Shader = Shader::Get("Collision2D.shader", false);
}

Doom::RectangleCollider2D::~RectangleCollider2D()
{
	
}

void RectangleCollider2D::CalculateRealVerPos() 
{
	if (this == nullptr) return;
	float* pSource;
	Transform* tr = GetOwnerOfComponent()->m_Transform;
	pSource = (float*)glm::value_ptr(tr->m_ViewMat4 * tr->m_ScaleMat4);
	for (unsigned int i = 0; i < 4; i++)
	{
		for (unsigned int j = 0; j < 4; j++)
		{
			m_TransformedVerPos[i * 4 + j] = 0;
			for (unsigned int k = 0; k < 4; k++)
			{
				m_TransformedVerPos[i * 4 + j] += m_Vertices[i * 2 + k] * pSource[k * 4 + j];
			}
		}
	}
	pSource = nullptr;
	CalculateEdges();
}

void Doom::RectangleCollider2D::CalculateEdges()
{
	glm::vec3 ownerPos = GetOwnerOfComponent()->GetPosition();
	p.clear();
	p.push_back(glm::vec2(m_TransformedVerPos[0]  + ownerPos.x, m_TransformedVerPos[1]  + ownerPos.y));
	p.push_back(glm::vec2(m_TransformedVerPos[4]  + ownerPos.x, m_TransformedVerPos[5]  + ownerPos.y));
	p.push_back(glm::vec2(m_TransformedVerPos[8]  + ownerPos.x, m_TransformedVerPos[9]  + ownerPos.y));
	p.push_back(glm::vec2(m_TransformedVerPos[12] + ownerPos.x, m_TransformedVerPos[13] + ownerPos.y));
}

float* RectangleCollider2D::GetVertexPositions()
{
	return m_TransformedVerPos;
}

void RectangleCollider2D::SetOffset(float x, float y) 
{
	m_Offset.x = x;
	m_Offset.y = y;
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

void RectangleCollider2D::IsCollidedSAT() 
{
	m_IsCollided = false;
	if (this == nullptr) return;
	if (m_Enable == true)
	{
		//for (size_t i = 0; i < 5; i++) //This is the temporary way to handle the error displacement of big ration of unit vector
		//{
		uint32_t sizeCol = s_Collision2d.size();
		for (unsigned int i = 0; i < sizeCol; i++)
		{
			if (s_Collision2d[i]->m_Enable == true) 
			{
				m_Col = s_Collision2d[i];
				if (this != m_Col) 
				{
					if (m_Col == nullptr) return;
					if (m_Col->m_IsTrigger) 
					{
						if (ShapeOverlap_SAT(*this, *m_Col)) 
						{
							m_IsCollided = true;
							m_CollidedObject = m_Col;
							//std::cout << "Handle Collision " << GetOwnerOfComponent()->m_Name << "\n";
							//std::function<void()> f2 = std::bind(&EventSystem::SendEvent, EventSystem::GetInstance(), EventType::ONCOLLISION, (Listener*)(m_OwnerOfCom), (void*)this->m_CollidedObject);
							//std::function<void()>* f1 = new std::function<void()>(f2);
							//EventSystem::GetInstance().SendEvent(EventType::ONMAINTHREADPROCESS, nullptr, f1);
							EventSystem::GetInstance().SendEvent(EventType::ONCOLLISION, m_OwnerOfCom->m_Listener, (void*)this->m_CollidedObject);
						}
						else 
						{
							m_IsCollided = false;
							m_CollidedObject = nullptr;
						}
					}
					else ShapeOverlap_SAT_STATIC(*this, *m_Col);
				}
			}
		//}
		}
	}
}

void RectangleCollider2D::IsCollidedDIAGS()
{
	m_IsCollided = false;
	m_CollidedObject = nullptr;
	if (this == nullptr) return;
	if (m_Enable == true) 
	{
		uint32_t sizeCol = s_Collision2d.size();
		for (unsigned int i = 0; i < sizeCol; i++)
		{
			if (s_Collision2d[i]->m_Enable == true) 
			{
				m_Col = s_Collision2d[i];
				if (this != m_Col) 
				{
					if (m_Col == nullptr) return;
					for (unsigned int p = 0; p < this->p.size(); p++)
					{
						glm::vec3 ownerPos = GetOwnerOfComponent()->m_Transform->GetPosition();
						glm::vec2 line_r1s = { ownerPos.x, ownerPos.y };
						glm::vec2 line_r1e = this->p[p];
						m_Displacement.x = 0;
						m_Displacement.y = 0;
						// ...against edges of the other
						for (unsigned int q = 0; q < m_Col->p.size(); q++)
						{
							glm::vec2 line_r2s = m_Col->p[q];
							glm::vec2 line_r2e = m_Col->p[(q + 1) % m_Col->p.size()];

							// Standard "off the shelf" line segment intersection
							float h = (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r1e.y) - (line_r1s.x - line_r1e.x) * (line_r2e.y - line_r2s.y);
							float t1 = ((line_r2s.y - line_r2e.y) * (line_r1s.x - line_r2s.x) + (line_r2e.x - line_r2s.x) * (line_r1s.y - line_r2s.y)) / h;
							float t2 = ((line_r1s.y - line_r1e.y) * (line_r1s.x - line_r2s.x) + (line_r1e.x - line_r1s.x) * (line_r1s.y - line_r2s.y)) / h;

							if (t1 >= 0.0f && t1 < 1.0f && t2 >= 0.0f && t2 < 1.0f)
							{
								if (m_Col->m_IsTrigger == false) {
									m_Displacement.x += (1.f - t1) * (line_r1e.x - line_r1s.x);
									m_Displacement.y += (1.f - t1) * (line_r1e.y - line_r1s.y);
								}
								m_CollidedObject = m_Col;
								m_IsCollided = true;
							}
						}
						if (m_IsTrigger == false) 
						{
							m_Transform = m_OwnerOfCom->m_ComponentManager.GetComponent<Transform>();
							m_Transform->Translate(m_OwnerOfCom->GetPosition().x - m_Displacement.x, m_OwnerOfCom->GetPosition().y - m_Displacement.y);
							m_Transform = nullptr;
						}
						
					}
				}
			}
		}
		return;
	}
}

void Doom::RectangleCollider2D::CollidersToInit()
{
	for (uint32_t i = 0; i < RectangleCollider2D::s_CollidersToInit.size(); i++)
	{
		Transform* tr = RectangleCollider2D::s_CollidersToInit[i]->GetOwnerOfComponent()->m_Transform;
		RectangleCollider2D* rc = RectangleCollider2D::s_CollidersToInit[i];
		rc->CalculateRealVerPos();
	}
	RectangleCollider2D::s_CollidersToInit.clear();
}

#include "Rays/Ray2D.h"
#include <limits>

bool RectangleCollider2D::ShapeOverlap_SAT_STATIC(RectangleCollider2D &r1, RectangleCollider2D &r2)
{
	RectangleCollider2D *poly1 = &r1;
	RectangleCollider2D *poly2 = &r2;

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

			if (!(max_r2 >= min_r1 && max_r1 >= min_r2)) return false;
		}
	}

	// If we got here, the objects have collided, we will displace r1
	// by overlap along the vector between the two object centers
	Transform* trans1 = r1.m_OwnerOfCom->m_ComponentManager.GetComponent<Transform>();
	Transform* trans2 = r2.m_OwnerOfCom->m_ComponentManager.GetComponent<Transform>();

	glm::vec2 r1Pos = trans1->GetPosition();
	glm::vec2 r2Pos = trans2->GetPosition();
	glm::vec2 d = glm::vec2(r2Pos.x - r1Pos.x, r2Pos.y - r1Pos.y);
	double s = sqrtf(d.x * d.x + d.y * d.y);

	//If displacement.x is set then even with 0 angle
	//of a collided object will move our player along the x axis
	//only if the both objects have not the same x
	//so it needs to be overthought either do we need continues collision detection or not

	glm::dvec2 posToTranslate = glm::vec2(0, 0);
	
	posToTranslate.x = r1.m_OwnerOfCom->GetPosition().x;
	float minAngle = glm::radians(1.0f);
	if (trans1->GetRotation().z > minAngle || trans2->GetRotation().z > minAngle)
		posToTranslate.x = r1Pos.x - (overlap * d.x / s);
	
	posToTranslate.y = r1Pos.y - (overlap * d.y / s);
	trans1->Translate(posToTranslate.x, posToTranslate.y);

	//std::function<void()> f2 = std::bind(&EventSystem::SendEvent, EventSystem::GetInstance(), EventType::ONCOLLISION, (m_OwnerOfCom->m_Listener), &r2);
	//std::function<void()>* f1 = new std::function<void()>(f2);
	//EventSystem::GetInstance().SendEvent(EventType::ONMAINTHREADPROCESS, nullptr, f1);
	EventSystem::GetInstance().SendEvent(EventType::ONCOLLISION, m_OwnerOfCom->m_Listener, (void*)&r2);
	return true;
}

bool RectangleCollider2D::ShapeOverlap_SAT(RectangleCollider2D &r1, RectangleCollider2D &r2)
{
	RectangleCollider2D *poly1 = &r1;
	RectangleCollider2D *poly2 = &r2;
	
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

			if (!(max_r2 >= min_r1 && max_r1 >= min_r2)) return false;
		}
	}
	return true;
}