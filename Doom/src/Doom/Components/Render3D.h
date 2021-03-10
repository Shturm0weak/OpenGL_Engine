#pragma once

#include "../Core/Core.h"
#include "../OpenGl/VertexArray.h"
#include "../OpenGl/VertexBuffer.h"
#include "../OpenGl/VertexBufferLayout.h"
#include "../OpenGl/IndexBuffer.h"
#include "../Render/MeshManager.h"

namespace Doom {

	class DOOM_API Renderer3D : public Irenderer {
	public:

		enum class RenderTechnic {
			Forward,
			Instancing
		};

		struct Material {
			float m_Ambient = 0.2f;
			float m_Specular = 0.2f;
			float m_Intensity = 0.0;
		};

		Material m_Material;
		Texture* m_DiffuseTexture = nullptr;
		Texture* m_NormalMapTexture = nullptr;
		Transform* m_Tr = nullptr;
		Mesh* m_Mesh = nullptr;
		RenderTechnic m_RenderTechnic = RenderTechnic::Forward;

	private:

		bool m_IsInitializedInInstancing = false;
	public:

		bool m_IsCastingShadows = true;
		bool m_IsWireMesh = false;
		bool m_IsUsingNormalMap = false;
		bool m_IsCullingFace = true;
		bool m_IsRenderable = true;

		std::map<std::string, float> m_FloatUniforms;
		
		Renderer3D(const Renderer3D& rhs);
		Renderer3D();
		~Renderer3D();

		static Component* Create();

		void operator=(const Renderer3D& rhs);
		void EraseFromInstancing();
		void ChangeRenderTechnic(RenderTechnic rt);
		void LoadMesh(Mesh* mesh);
		void BakeShadows();
		void MakeTransparent();
		void MakeSolid();
		virtual void Render() override;
		virtual void Delete() override;

	private:

		static std::map<char*, uint64_t> s_MemoryPool;
		static std::vector<char*> s_FreeMemory;
		char* m_MemoryPoolPtr = nullptr;
		//TODO ??? Upload in shader, calculate the angle between the normal and light dir if angle > 90 then shadow should not be drawn!!! ???
		//TODO Make bake shadows strength depending on alpha channel of color for transparent object!!!
		bool m_IsTransparent = false;
		bool m_IsSkyBox = false;

		void Copy(const Renderer3D& rhs);
		void ForwardRender(glm::mat4& pos, glm::mat4& view, glm::mat4& scale, glm::vec4& color);
		void RenderSkyBox();
		void AdditionalUniformsLoad();

		friend class Doom::Instancing;
		friend class Doom::SceneSerializer;
		friend class Doom::EntryPoint;
		friend class Doom::SkyBox;
		friend class Doom::MeshManager;
		friend class Doom::ComponentManager;
		friend class Doom::Editor;
		friend class Doom::Renderer;
	};

}