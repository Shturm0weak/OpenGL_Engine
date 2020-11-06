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

		std::map<std::string, float*> floatUniforms;

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

		void BakeShadows();

		virtual void Render() override;

	private:

		bool isSkyBox = false;

		void ForwardRender();
		void AdditionalUniformsLoad();

		RenderTechnic renderTechnic = RenderTechnic::Forward;

		friend class EntryPoint;
		friend class SkyBox;
		friend class MeshManager;
	};

}

#endif