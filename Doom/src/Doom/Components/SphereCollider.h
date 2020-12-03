#pragma once

namespace Doom {

	class DOOM_API SphereCollider : public Component {
	private:

		Mesh* m_Mesh = nullptr;
		Shader* m_Shader = Shader::Get("CubeCollider");
		glm::vec4 m_Color = COLORS::Green;

		friend class ComponentManager;
	public:

		static std::vector<SphereCollider*> spheres;
		glm::vec3 m_Offset = { 0,0,0 };
		float m_Radius = 1.0;
		bool m_IsInBoundingBox = true;
		
		SphereCollider();
		~SphereCollider();

		void Render();
		bool IntersectSphereToSphere(SphereCollider* sp);
	};
}