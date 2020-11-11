#pragma once
#ifndef RENDERER3D_H
#define RENDERER3D_H

#include "../Core/Core.h"
#include "../Render/VertexArray.h"
#include "../Render/VertexBuffer.h"
#include "../Render/VertexBufferLayout.h"
#include "../Render/IndexBuffer.h"
#include "../Render/MeshManager.h"

namespace Doom {

	class DOOM_API Renderer3D : public Irenderer {
	public:

		std::map<std::string, float> floatUniforms;

		enum class RenderTechnic {
			Forward,
			Instancing
		};

		struct Material {
			float ambient = 0.2f;
			float specular = 0.2f;
		};

		void ChangeRenderTechnic(RenderTechnic rt);
		void LoadMesh(Mesh* mesh);
		bool useNormalMap = false;
		Texture* diffuseTexture = Texture::WhiteTexture;
		Texture* normalMapTexture = Texture::Get("InvalidTexture");

		void EraseFromInstancing();

		Transform* tr = nullptr;

		Mesh* mesh = nullptr;

		Renderer3D(GameObject* _owner);
		~Renderer3D();

		Material mat;
		bool isCastingShadows = true;
		bool isWireMesh = false;
		void BakeShadows();
		void MakeTransparent();
		void MakeSolid();
		virtual void Render() override;
	private:

		//TODO ??? Upload in shader, calculate the angle between the normal and light dir if angle > 90 then shadow should not be drawn!!! ???
		//TODO Make bake shadows strength depending on alpha channel of color for transparent object!!!
		bool isTransparent = false;
		bool isSkyBox = false;

		void ForwardRender(glm::mat4& pos, glm::mat4& view, glm::mat4& scale, glm::vec4& color);
		void AdditionalUniformsLoad();

		RenderTechnic renderTechnic = RenderTechnic::Forward;

		friend class SceneSerializer;
		friend class Doom::EntryPoint;
		friend class Doom::SkyBox;
		friend class Doom::MeshManager;
		friend class Doom::ComponentManager;
		friend class Doom::Editor;
	};

}

#endif