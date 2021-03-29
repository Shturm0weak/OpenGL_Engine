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

		struct Stats {
			uint64_t m_Vertices = 0;
			uint64_t m_DrawCalls = 0;
			double m_RenderTime = 0;
			double m_ObjectsRenderTime = 0;
			double m_GuiRenderTime = 0;
			double m_ShadowRenderTime = 0;
			double m_BloomRenderTime = 0;
			double m_CollisionRenderTime = 0;
			uint64_t m_FramePerSecond = 0;

			void Reset() {
				m_Vertices = 0;
				m_DrawCalls = 0;
				m_RenderTime = 0;
				m_ObjectsRenderTime = 0;
				m_GuiRenderTime = 0;
				m_ShadowRenderTime = 0;
				m_BloomRenderTime = 0;
				m_CollisionRenderTime = 0;
				m_FramePerSecond = 0;
			}
		};

		static Stats s_Stats;
		static bool s_PolygonMode;
		static float s_Exposure;
		static bool s_BloomEffect;
		static float s_Brightness;

	private:

		static std::vector <SpriteRenderer*> s_Objects2d;
		static std::vector <Renderer3D*> s_Objects3d;
		static std::vector <Renderer3D*> s_Objects3dTransparent;
		static std::vector <Renderer3D*> s_OutLined3dObjects;

		static void Render2DObjects();
		static void Render3DObjects();
		static void Render3D();
		static void Render2D();
		static void BakeShadows();
		static void UpdateLightSpaceMatrices();
		static void RenderCollision3D();
		static void Render();
		static void RenderScene();
		static void RenderLines();
		static void RenderText();
		static void RenderTransparent();
		static void RenderCollision();
		static void Clear();
		static void SortTransparentObjects();
		static void RenderForPostEffect(Mesh* mesh, Shader* shader);
		static void RenderBloomEffect();
		static void RenderOutLined3dObjects();

		friend class EntryPoint;
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