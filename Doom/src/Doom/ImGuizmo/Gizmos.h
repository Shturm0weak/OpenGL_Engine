#pragma once
#include "../Render/Line.h"

namespace Doom {

	class Gizmos : public GameObject {
	private:
		std::mutex mtx;
		bool Intersect(float& tMin, glm::vec3 bMin, glm::vec3 bMax, glm::vec3 start, glm::vec3 dir);
		void IntersectLine(glm::vec3 dir, float scale);
		void IntersectPlane(glm::vec3 dir);
		std::vector<GameObject*> faces;
		std::vector <Line*> axis;
		Line* l = nullptr;
		GameObject* f = nullptr;
	public:
		bool Enable = false;
		bool roundTransform = false;
		bool isHovered;
		bool blockFrame = false;
		Transform* tr;
		GameObject* obj = nullptr;

		Gizmos(float x = 0, float y = 0,float z = 0);
		
		virtual void OnUpdate() override;
		void Render();
	};

}