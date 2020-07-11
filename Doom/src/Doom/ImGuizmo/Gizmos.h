#pragma once
#include "../Render/Line.h"

namespace Doom {

	class Gizmos : public GameObject {
	private:
		std::pair<float, GameObject*> Intersect(glm::vec3 dir);
		std::vector<GameObject*> faces;
		std::vector <Line*> axis;
	public:
		bool Enable = false;

		Transform* tr;
		GameObject* obj = nullptr;

		Gizmos(float x = 0, float y = 0,float z = 0);
		
		virtual void OnUpdate() override;
		void Render();
	};

}