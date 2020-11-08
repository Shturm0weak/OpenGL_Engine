#pragma once
#include "../Core/Core.h"
#include "../ECS/Component.h"

namespace Doom {

	class DOOM_API CubeCollider3D : public Component {
		
		Mesh* mesh = nullptr;
		glm::vec4 color = COLORS::Green;

		Shader* shader = Shader::Get("CubeCollider");

		VertexBufferLayout* layout = nullptr;
		VertexBuffer* vb = nullptr;
		VertexArray* va = nullptr;
		IndexBuffer* ib = nullptr;
	public:
		glm::vec3 minP = { -1,-1,-1 };
		glm::vec3 maxP = {  1, 1, 1 };
		glm::vec3 offset = {0,0,0};
		bool isBoundingBox = false;
		static std::vector<CubeCollider3D*> colliders;
		static void InitMesh();
		~CubeCollider3D();
		CubeCollider3D();
		void Render();
	};

}