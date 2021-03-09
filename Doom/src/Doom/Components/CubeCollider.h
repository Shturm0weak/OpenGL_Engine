#pragma once

#include "../Core/Core.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API CubeCollider3D : public Component {

		static std::map<char*, uint64_t> s_MemoryPool;
		static std::vector<char*> s_FreeMemory;
		
		glm::vec4 m_Color = COLORS::Green;
		Mesh* m_Mesh = MeshManager::GetInstance().GetMesh("CubeCollider");
		Shader* m_Shader = Shader::Get("CubeCollider");

		char* m_MemoryPoolPtr = nullptr;
	public:

		glm::vec3 m_MinP = { -1,-1,-1 };
		glm::vec3 m_MaxP = { 1, 1, 1 };
		glm::vec3 m_Offset = { 0, 0, 0 };
		bool m_IsBoundingBox = false;
		static std::vector<CubeCollider3D*> s_Colliders;

		static Component* Create();
		virtual void Delete() override;
		static void InitMesh();
		void Render();

		~CubeCollider3D();
		CubeCollider3D();

		friend class ComponentManager;
	};

}