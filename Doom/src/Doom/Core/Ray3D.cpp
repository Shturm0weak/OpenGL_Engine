#include "../pch.h"
#include "Ray3D.h"

std::map<float, Doom::CubeCollider3D*> Doom::Ray3D::RayCast(glm::vec3 start, glm::vec3 dir, Hit * hit,float length)
{
	std::map<float, CubeCollider3D*> d;
	size_t size = CubeCollider3D::colliders.size();
	for (size_t i = 0; i < size; i++)
	{
		CubeCollider3D* c = CubeCollider3D::colliders[i];
		glm::vec3 pos = c->offset + c->GetOwnerOfComponent()->GetPositions();
		glm::vec3 scale = c->scale * c->GetOwnerOfComponent()->GetScale();
		glm::vec3 bMin = pos + (glm::vec3(-1,-1,-1) * scale);
		glm::vec3 bMax = pos + (glm::vec3(1, 1, 1) * scale);

		float txMin = (bMin.x - start.x) / dir.x;
		float txMax = (bMax.x - start.x) / dir.x;
		if (txMax < txMin) {
			float temp = txMax;
			txMax = txMin;
			txMin = temp;
		}

		float tyMin = (bMin.y - start.y) / dir.y;
		float tyMax = (bMax.y - start.y) / dir.y;
		if (tyMax < tyMin) {
			float temp = tyMax;
			tyMax = tyMin;
			tyMin = temp;
		}

		float tzMin = (bMin.z - start.z) / dir.z;
		float tzMax = (bMax.z - start.z) / dir.z;
		if (tzMax < tzMin) {
			float temp = tzMax;
			tzMax = tzMin;
			tzMin = temp;
		}

		float tMin = (txMin > tyMin) ? txMin : tyMin;
		float tMax = (txMax < tyMax) ? txMax : tyMax;

		if (txMin > tyMax || tyMin > txMax) continue;
		if (tMin > tzMax || tzMin > tMax) continue;
		if (tzMin > tMin) tMin = tzMin;
		if (tzMax < tMax) tMax = tzMax;
		d.insert(std::make_pair(tMin, c));
	}

	float min = 1e300;
	for (auto i = d.begin(); i != d.end(); i++)
	{
		if (i->first < min)
			min = i->first;
	}

	Doom::Ray3D::sortMap(d);

	auto iter = d.find(min);
	if (iter != d.end()) {
		hit->Object = iter->second;
		hit->point = dir * iter->first;
		hit->distance = iter->first;
	}
	else {
		hit->Object = nullptr;
		hit->point = glm::vec3(0.0f);
		hit->distance = 0;
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
	if (vp >= -0.0001 && vp <= 0.0001) {
		return false;
	}
	float wp = glm::dot(rayToPlaneDelta, planeNorm);
	float t = wp / vp;
	glm::vec3 iPos = rayDelta * t + start;
	hit->point = iPos;

	glm::vec3 edge0 = b - a;
	glm::vec3 edge1 = c - b;
	glm::vec3 edge2 = a - c;
	glm::vec3 c0 = iPos - a;
	glm::vec3 c1 = iPos - b;
	glm::vec3 c2 = iPos - c;
	
	if (glm::dot(planeNorm, glm::cross(edge0, c0)) > 0 &&
		glm::dot(planeNorm, glm::cross(edge1, c1)) > 0 &&
		glm::dot(planeNorm, glm::cross(edge2, c2)) > 0) {
		return true;
	}
	else {
		return false;
	}
}

void Doom::Ray3D::Normilize(glm::vec3 & vector)
{
		vector = glm::vec3(vector * (1.f / sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z)));
}

void Doom::Ray3D::sortMap(std::map<float, CubeCollider3D*>& M)
{
	std::vector<std::pair<float, CubeCollider3D*> > A;

	for (auto& it : M) {
		A.push_back(it);
	}
	sort(A.begin(), A.end(), [](pair<float, CubeCollider3D*>& a,
		pair<float, CubeCollider3D*>& b) {return a.first < b.first; });
}