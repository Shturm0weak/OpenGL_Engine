#pragma once
#ifndef RENDERER3D_H
#define RENDERER3D_H

#include "../Core/Core.h"
#include "../OpenGl/VertexArray.h"
#include "../OpenGl/VertexBuffer.h"
#include "../OpenGl/VertexBufferLayout.h"
#include "../OpenGl/IndexBuffer.h"
#include "../Render/MeshManager.h"

namespace Doom {

	class DOOM_API Renderer3D : public Irenderer {
	public:

		std::map<std::string, float> m_FloatUniforms;

		enum class RenderTechnic {
			Forward,
			Instancing
		};

		struct Material {
			float m_Ambient = 0.2f;
			float m_Specular = 0.2f;
		};

		Material m_Material;
		Texture* m_DiffuseTexture = Texture::s_WhiteTexture;
		Texture* m_NormalMapTexture = Texture::Get("InvalidTexture");
		Transform* m_Tr = nullptr;
		Mesh* m_Mesh = nullptr;
		bool m_IsCastingShadows = true;
		bool m_IsWireMesh = false;
		bool m_IsUsingNormalMap = false;

		Renderer3D();
		~Renderer3D();

		void EraseFromInstancing();
		void ChangeRenderTechnic(RenderTechnic rt);
		void LoadMesh(Mesh* mesh);
		void BakeShadows();
		void MakeTransparent();
		void MakeSolid();
		virtual void Render() override;
	private:

		//TODO ??? Upload in shader, calculate the angle between the normal and light dir if angle > 90 then shadow should not be drawn!!! ???
		//TODO Make bake shadows strength depending on alpha channel of color for transparent object!!!
		bool m_IsTransparent = false;
		bool m_IsSkyBox = false;
		RenderTechnic m_RenderTechnic = RenderTechnic::Forward;

		void ForwardRender(glm::mat4& pos, glm::mat4& view, glm::mat4& scale, glm::vec4& color);
		void AdditionalUniformsLoad();

		friend class Doom::SceneSerializer;
		friend class Doom::EntryPoint;
		friend class Doom::SkyBox;
		friend class Doom::MeshManager;
		friend class Doom::ComponentManager;
		friend class Doom::Editor;
	};

}

#endif