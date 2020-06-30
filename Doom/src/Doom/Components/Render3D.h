#pragma once
#ifndef RENDERER3D_H
#define RENDERER3D_H

#include "../Core/Core.h"
#include "../Render/VertexArray.h"
#include "../Render/VertexBuffer.h"
#include "../Render/VertexBufferLayout.h"
#include "../Render/IndexBuffer.h"
#include "../Render/Mesh.h"

namespace Doom {

	class DOOM_API Renderer3D : public Irenderer {
	public:

		struct Material {
			float ambient = 0.2f;
			float specular = 0.2f;
		};

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
		VertexBufferLayout* layout = nullptr;
		VertexBuffer* vb = nullptr;
		VertexArray* va = nullptr;
		IndexBuffer* ib = nullptr;

	};

}

#endif // !RENDERER3D_H