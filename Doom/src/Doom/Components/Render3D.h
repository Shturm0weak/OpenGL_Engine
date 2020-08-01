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

		enum class RenderTechnic {
			Forward,
			Instancing
		};

		struct Material {
			float ambient = 0.2f;
			float specular = 0.2f;
		};

		void ChangeRenderTechnic(RenderTechnic rt);
		bool useNormalMap = false;
		Texture* diffuseTexture = Texture::WhiteTexture;
		Texture* normalMapTexture = nullptr;

		Transform* tr = nullptr;

		void LoadMesh(Mesh* mesh);

		Shader* shader = nullptr;
		Mesh* mesh = nullptr;

		Renderer3D(GameObject* _owner);
		~Renderer3D();

		Material mat;

		virtual void Render() override;

	private:

		bool isSkyBox = false;

		void ForwardRender();

		RenderTechnic renderTechnic = RenderTechnic::Forward;

		friend class EntryPoint;
		friend class SkyBox;
	};

}

#endif