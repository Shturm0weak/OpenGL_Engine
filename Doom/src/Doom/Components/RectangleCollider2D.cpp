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
	component->p.reserve(4);
	World::GetInstance().s_ColId++;
	std::function<void()>* f = new std::function<void()>([=] {
		component->CalculateRealVerPos();
		EventSystem::GetInstance().RegisterClient(EventType::ONMOVE, (Listener*)component);
		EventSystem::GetInstance().RegisterClient(EventType::ONROTATE, (Listener*)component);
		EventSystem::GetInstance().RegisterClient(EventType::ONSCALE, (Listener*)component);
		EventSystem::GetInstance().RegisterClient(EventType::ONTRANSLATE, (Listener*)component);
		});
	EventSystem::GetInstance().SendEvent(EventType::ONMAINTHREADPROCESS, nullptr, f);
	return component;
}

RectangleCollider2D::RectangleCollider2D()
{
}

Doom::RectangleCollider2D::~RectangleCollider2D()
{
	
}

void RectangleCollider2D::CalculateRealVerPos() 
{
	float* pSource = (float*)glm::value_ptr(m_OwnerOfCom->m_Transform.m_ViewMat4 * m_OwnerOfCom->m_Transform.m_ScaleMat4);
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			m_WorldSpaceVertices[i * 4 + j] = 0;
			for (size_t k = 0; k < 4; k++)
			{
				m_WorldSpaceVertices[i * 4 + j] += m_Vertices[i * 2 + k] * pSource[k * 4 + j];
			}
		}
	}
	CalculateEdges();
}

void Doom::RectangleCollider2D::CalculateEdges()
{
	glm::vec3 position = m_OwnerOfCom->GetPosition();
	p.clear();
	p.push_back(glm::vec2(m_WorldSpaceVertices[0]  + position.x, m_WorldSpaceVertices[1]  + position.y));
	p.push_back(glm::vec2(m_WorldSpaceVertices[4]  + position.x, m_WorldSpaceVertices[5]  + position.y));
	p.push_back(glm::vec2(m_WorldSpaceVertices[8]  + position.x, m_WorldSpaceVertices[9]  + position.y));
	p.push_back(glm::vec2(m_WorldSpaceVertices[12] + position.x, m_WorldSpaceVertices[13] + position.y));
}

float* RectangleCollider2D::GetVertexPositions()
{
	return m_WorldSpaceVertices;
}

void RectangleCollider2D::SetOffset(float x, float y) 
{
	SetOffset(glm::vec2(x, y));
}

void Doom::RectangleCollider2D::SetOffset(glm::vec2 offset)
{
	m_Offset = offset;
}

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
							//std::cout << "Handle Collision " << m_OwnerOfCom->m_Name << "\n";
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
						glm::vec3 ownerPos = m_OwnerOfCom->m_Transform.GetPosition();
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
							Transform* tr = m_OwnerOfCom->m_ComponentManager.GetComponent<Transform>();
							tr->Translate(m_OwnerOfCom->GetPosition().x - m_Displacement.x, m_OwnerOfCom->GetPosition().y - m_Displacement.y);
						}
					}
				}
			}
		}
		return;
	}
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