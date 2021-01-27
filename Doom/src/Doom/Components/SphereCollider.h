#pragma once

namespace Doom {

	class DOOM_API SphereCollider : public Component {
	public:

		static std::vector<SphereCollider*> s_Spheres;
	private:

		glm::vec4 m_Color = COLORS::Green;

		Mesh* m_Mesh = nullptr;
		Shader* m_Shader = Shader::Get("CubeCollider");

		void Copy(const SphereCollider& rhs);

		friend class ComponentManager;
	public:

		glm::vec3 m_Offset = { 0,0,0 };
		float m_Radius = 1.0;
		bool m_IsInBoundingBox = true;
		
		void operator=(const SphereCollider& rhs);

		static Component* Create();

		SphereCollider(const SphereCollider& rhs);
		SphereCollider();
		~SphereCollider();

		void Render();
		bool IntersectSphereToSphere(SphereCollider* sp);
	};
}