#pragma once
#include "../Objects/Line.h"

namespace Doom {

	class Gizmos : public GameObject {
	private:
	
		std::vector<GameObject*> m_Faces;
		std::vector <Line*> m_Axis;
		Line* m_L = nullptr;
		GameObject* m_F = nullptr;

		bool Intersect(float& tMin, glm::vec3 bMin, glm::vec3 bMax, glm::vec3 start, glm::vec3 dir);
		void IntersectLine(glm::vec3 dir, float scale);
		void IntersectPlane(glm::vec3 dir);

	public:

		Transform* m_Tr;
		GameObject* m_Obj = nullptr;
		bool m_Enable = false;
		bool m_RoundTransform = false;
		bool m_IsHovered;
		bool m_BlockFrame = false;
		
		Gizmos(float x = 0, float y = 0,float z = 0);
		
		virtual void OnUpdate() override;
		void Render();
	};

}