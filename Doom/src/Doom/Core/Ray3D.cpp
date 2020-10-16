#include "../pch.h"
#include "Ray3D.h"

void Doom::Ray3D::RayCast(glm::vec3 start, glm::vec3 dir, Hit * hit,float length)
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

}
