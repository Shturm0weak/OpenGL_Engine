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
		static int Vertices;
		static bool PolygonMode;
		static int DrawCalls;
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
		inline static unsigned int GetAmountOfCollisions() { return collision2d.size(); }
		static const char** items;

	private:
		static std::vector <RectangleCollider2D*> collision2d;
		static std::vector <SpriteRenderer*> objects2d;
		static std::vector <Renderer3D*> objects3d;
		static std::vector <Renderer3D*> objects3dTransparent;
		static std::mutex mtx;
		static std::condition_variable condVar;

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