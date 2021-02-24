#include "../pch.h"
#include "Ray3D.h"
#include "../Objects/Line.h"
std::map<float, Doom::CubeCollider3D*> Doom::Ray3D::RayCast(glm::vec3 start, glm::vec3 dir, Hit * hit,float length, bool AABB, std::vector<std::string> ignoreMask)
{
	std::map<float, CubeCollider3D*> d;
	size_t collidersSize = CubeCollider3D::s_Colliders.size();
	for (size_t i = 0; i < collidersSize; i++)
	{
		bool hasTag = ignoreMask.size() > 0 ? false : true;
		for (size_t j = 0; j < ignoreMask.size(); j++)
		{
			if (CubeCollider3D::s_Colliders[i]->GetOwnerOfComponent()->m_Tag == ignoreMask[j]) 
			{
				hasTag = true;
				break;
			}
		}

		if (!hasTag)
			continue;

		bool isIntersected = false;
		if(AABB)
			isIntersected = IntersectBoxAABB(start, dir, hit, length, CubeCollider3D::s_Colliders[i]);
		else
			isIntersected = IntersectBoxOBB(start, dir, hit, length, CubeCollider3D::s_Colliders[i]);
		if (isIntersected)
			d.insert(std::make_pair(hit->m_Distance, hit->m_Object));
	}

	if(d.size() > 0)
	{
		hit->m_Object = d.begin()->second;
		hit->m_Point = dir * d.begin()->first;
		hit->m_Distance = d.begin()->first;
	}
	else 
	{
		hit->m_Object = nullptr;
		hit->m_Point = glm::vec3(0.0f);
		hit->m_Distance = 0;
	}
	return d;
}

bool Doom::Ray3D::IntersectTriangle(glm::vec3 start, glm::vec3 dir, Hit * hit, float length, glm::vec3& a, glm::vec3& b, glm::vec3& c, glm::vec3& planeNorm)
{
	glm::vec3 end = dir * length; 
	glm::vec3 rayDelta = end - start;
	glm::vec3 rayToPlaneDelta = a - start;
	float ratio = glm::dot(rayToPlaneDelta,planeNorm);
	glm::vec3 proj = planeNorm * ratio;
	float vp = glm::dot(rayDelta,planeNorm);
	if (vp >= -0.0001 && vp <= 0.0001)
	{
		return false;
	}
	float wp = glm::dot(rayToPlaneDelta, planeNorm);
	float t = wp / vp;
	glm::vec3 iPos = rayDelta * t + start;
	hit->m_Point = iPos;

	glm::vec3 edge0 = b - a;
	glm::vec3 edge1 = c - b;
	glm::vec3 edge2 = a - c;
	glm::vec3 c0 = iPos - a;
	glm::vec3 c1 = iPos - b;
	glm::vec3 c2 = iPos - c;
	
	if (glm::dot(planeNorm, glm::cross(edge0, c0)) > 0 &&
		glm::dot(planeNorm, glm::cross(edge1, c1)) > 0 &&
		glm::dot(planeNorm, glm::cross(edge2, c2)) > 0) return true;
	else return false;
}

void Doom::Ray3D::Normilize(glm::vec3 & vector)
{
		vector = glm::vec3(vector * (1.f / sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z)));
}

bool Doom::Ray3D::IntersectBoxAABB(glm::vec3 start, glm::vec3 dir, Hit * hit, float length, CubeCollider3D * c)
{
	glm::vec3 pos = c->m_Offset * c->GetOwnerOfComponent()->GetScale() + c->GetOwnerOfComponent()->GetPosition();
	glm::vec3 scale = glm::abs(c->m_MinP) + glm::abs(c->m_MaxP);
	glm::vec3 bMin = pos + (glm::vec3(-1, -1, -1) * scale);
	glm::vec3 bMax = pos + (glm::vec3( 1,  1,  1) * scale);

	float txMin = (bMin.x - start.x) / dir.x;
	float txMax = (bMax.x - start.x) / dir.x;
	if (txMax < txMin) 
	{
		float temp = txMax;
		txMax = txMin;
		txMin = temp;
	}

	float tyMin = (bMin.y - start.y) / dir.y;
	float tyMax = (bMax.y - start.y) / dir.y;
	if (tyMax < tyMin)
	{
		float temp = tyMax;
		tyMax = tyMin;
		tyMin = temp;
	}

	float tzMin = (bMin.z - start.z) / dir.z;
	float tzMax = (bMax.z - start.z) / dir.z;
	if (tzMax < tzMin) 
	{
		float temp = tzMax;
		tzMax = tzMin;
		tzMin = temp;
	}

	float tMin = (txMin > tyMin) ? txMin : tyMin;
	float tMax = (txMax < tyMax) ? txMax : tyMax;

	if (txMin > tyMax || tyMin > txMax) return false;
	if (tMin > tzMax || tzMin > tMax) return false;
	if (tzMin > tMin) tMin = tzMin;
	if (tzMax < tMax) tMax = tzMax;
	hit->m_Distance = tMin;
	hit->m_Object = c;
	hit->m_Point = tMin * dir + start;
	return true;
}



bool Doom::Ray3D::IntersectBoxOBB(glm::vec3 start, glm::vec3 dir, Hit* hit, float length, CubeCollider3D* c)
{
	Transform* tr = c->GetOwnerOfComponent()->GetComponent<Transform>();
	glm::vec3 bounds0 = c->m_MinP;
	glm::vec3 bounds1 = c->m_MaxP;
	glm::vec3 vPos = tr->GetPosition() + c->m_Offset * tr->GetScale();
	glm::mat4 pos = glm::translate(glm::mat4(1.0f), vPos);
	glm::mat4 wMat = pos * tr->m_ViewMat4;
	bounds0 = glm::vec3(tr->m_ScaleMat4 * glm::vec4(bounds0, 1.0f));
	bounds1 = glm::vec3(tr->m_ScaleMat4 * glm::vec4(bounds1, 1.0f));
	float* wMatPtr = glm::value_ptr(wMat);
	glm::vec3 axis;
	glm::vec3 bbRayDelta = vPos - start;

	float tMin = 0, tMax = 1000000, nomLen, denomLen, tmp, min, max;
	size_t p;

	for (size_t i = 0; i < 3; i++)
	{
		p = i * 4;
		axis = glm::vec3(wMatPtr[p], wMatPtr[p + 1], wMatPtr[p + 2]);
		glm::normalize(axis);
		nomLen = glm::dot(axis, bbRayDelta);
		denomLen = glm::dot(dir, axis);
		if (glm::abs(denomLen) > 0.00001)
		{

			min = (nomLen + bounds0[i]) / denomLen;
			max = (nomLen + bounds1[i]) / denomLen;

			if (min > max) { tmp = min; min = max; max = tmp; }
			if (min > tMin) tMin = min;
			if (max < tMax) tMax = max;

			if (tMax < tMin) return false;

		}
		else if (-nomLen + bounds0[i] > 0 || -nomLen + bounds1[i] < 0) return false;
	}

	hit->m_Distance = tMin;
	hit->m_Object = c;
	hit->m_Point = tMin * dir + start;
	return true;
}