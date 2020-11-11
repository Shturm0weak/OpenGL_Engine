#pragma once

namespace Doom {

	class DOOM_API SphereCollider : public Component {
	public:
		float radius = 1.0;
		SphereCollider();
		~SphereCollider();
	private:
		friend class ComponentManager;

		Mesh* mesh = nullptr;
		glm::vec4 color = COLORS::Green;
		Shader* shader = Shader::Get("CubeCollider");
	public:
		bool isInBoundingBox = true;
		glm::vec3 offset = { 0,0,0 };
		static std::vector<SphereCollider*> spheres;
		void Render();
		bool IntersectSphereToSphere(SphereCollider* sp);
	};
}