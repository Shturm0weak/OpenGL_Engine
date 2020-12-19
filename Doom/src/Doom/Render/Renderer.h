#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <mutex>
#include "../Components/RectangleCollider2D.h"
#include "../Components/SpriteRenderer.h"
#include "../Components/Render3D.h"

namespace Doom {

	class DOOM_API Renderer {
	public:

		static int s_Vertices;
		static bool s_PolygonMode;
		static int s_DrawCalls;
		static float s_Exposure;
		static bool s_BloomEffect;
		static float s_Brightness;

		static void Render2DObjects();
		static void Render3DObjects();
		static void BakeShadows();
		static void UpdateLightSpaceMatrices();
		static void RenderCollision3D();
		static void Render();
		static void RenderLines();
		static void RenderText();
		static void RenderTransparent();
		static void RenderCollision();
		static void Clear();
		static void SortTransparentObjects();
		static void RenderForPostEffect(Mesh* mesh, Shader* shader);
		static void RenderBloomEffect();
		inline static unsigned int GetAmountOfCollisions() { return s_Collision2d.size(); }
	private:

		static std::vector <RectangleCollider2D*> s_Collision2d;
		static std::vector <SpriteRenderer*> s_Objects2d;
		static std::vector <Renderer3D*> s_Objects3d;
		static std::vector <Renderer3D*> s_Objects3dTransparent;

		friend class Renderer2DLayer;
		friend class GameObject;
		friend class SpriteRenderer;
		friend class Editor;
		friend class ComponentManager;
		friend class Ray2D;
		friend class RectangleCollider2D;
		friend class Renderer3D;
	};

}

#endif