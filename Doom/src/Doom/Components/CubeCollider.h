#pragma once

#include "../Core/Core.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API CubeCollider3D : public Component {
		
		glm::vec4 m_Color = COLORS::Green;
		Mesh* m_Mesh = nullptr;
		Shader* m_Shader = Shader::Get("CubeCollider");

		VertexBufferLayout* m_Layout = nullptr;
		VertexBuffer* m_Vb = nullptr;
		VertexArray* m_Va = nullptr;
		IndexBuffer* m_Ib = nullptr;
	public:

		glm::vec3 m_MinP = { -1,-1,-1 };
		glm::vec3 m_MaxP = { 1, 1, 1 };
		glm::vec3 m_Offset = { 0, 0, 0 };
		bool m_IsBoundingBox = false;
		static std::vector<CubeCollider3D*> s_Colliders;

		static Component* Create();

		static void InitMesh();
		void Render();

		~CubeCollider3D();
		CubeCollider3D();
	};

}