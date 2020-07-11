#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <mutex>
#include "../Components/RectangleCollider2D.h"

namespace Doom {

	class DOOM_API Renderer {
	public:

		//
		static GameObject* Light;
		//
		static int Vertices;
		static bool PolygonMode;
		static int DrawCalls;
		static bool isReadyToRenderFirstThread;
		static bool isReadyToRenderSecondThread;
		static bool isReadyToRenderThirdThread;
		static void DeleteObject(int id);
		static void DeleteAll();
		static void Render2DObjects();
		static void Render3DObjects();
		static void Render();
		static void CalculateMVPforAllObjects();
		static void RenderLines();
		static void RenderText();
		static void RenderCollision();
		static void Clear();
		static void Save(const std::string filename);
		static void Load(const std::string filename);
		static GameObject* SelectObject();
		static std::vector<unsigned int> CalculateObjectsVectors();
		inline static std::vector<unsigned int> GetObjectsWithNoOwnerReference() { return ObjectsWithNoOwner; }
		inline static std::vector<unsigned int> GetObjectsWithOwnerReference() { return ObjectsWithOwner; }
		inline static unsigned int GetAmountOfObjects() { return objects2d.size(); }
		inline static unsigned int GetAmountOfCollisions() { return collision2d.size(); }
		static GameObject* GetReference(int id) { return objects2d[id]; }
		static const char** items;
		static void ShutDown();
		static void PopBack() { objects2d.pop_back(); }
		static const char** GetItems();
		static int obj_id;
		static int col_id;
		static GameObject* CreateGameObject();

	private:
		static bool ObjectCollided(std::vector<glm::vec2>& p,int i);
		static std::vector<unsigned int> ObjectsWithNoOwner;
		static std::vector<unsigned int> ObjectsWithOwner;
		static std::vector <RectangleCollider2D*> collision2d;
		static std::vector <GameObject*> objects2d;
		static std::vector <GameObject*> objects3d;
		static std::mutex mtx;
		static std::condition_variable condVar;

		friend class Renderer2DLayer;
		friend class GameObject;
		friend class SpriteRenderer;
		friend class Editor;
		friend class ComponentManager;
		friend class Ray2D;
		friend class RectangleCollider2D;

		template<typename T>
		static void LoadObj(bool enable,std::string name, std::string pathtotext, float angle, float color[4],
			float scale[3], double pos[2], bool hascollision, float* offset, int* axes, bool istrigger, std::string tag,float* uvs,bool isSprite,float* spriteSize) {
			T* go = new T(name, pos[0], pos[1]);
			go->Enable = enable;
			go->GetComponentManager()->GetComponent<Transform>()->Translate(pos[0],pos[1],pos[2]);
			go->GetComponentManager()->GetComponent<Transform>()->Scale(scale[0], scale[1],scale[2]);
			go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(angle,angle,angle,true);
			SpriteRenderer* sr = static_cast<SpriteRenderer*>(go->GetComponentManager()->GetComponent<Irenderer>());
			sr->mesh2D[2] = uvs[0];
			sr->mesh2D[3] = uvs[1];
			sr->mesh2D[6] = uvs[2];
			sr->mesh2D[7] = uvs[3];
			sr->mesh2D[10] = uvs[4];
			sr->mesh2D[11] = uvs[5];
			sr->mesh2D[14] = uvs[6];
			sr->mesh2D[15] = uvs[7];
			go->GetComponentManager()->GetComponent<Irenderer>()->SetColor(glm::vec4(color[0], color[1], color[2], color[3]));
			if(pathtotext != "None")
				sr->SetTexture(pathtotext);
			if (isSprite) {
				for (unsigned int i = 0; i < TextureAtlas::textureAtlases.size(); i++)
				{
					if (TextureAtlas::textureAtlases[i]->m_texture->GetFilePath() == pathtotext) {
						go->GetComponentManager()->GetComponent<Irenderer>()->textureAtlas = TextureAtlas::textureAtlases[i];
					}
				}
				if(go->GetComponentManager()->GetComponent<Irenderer>()->textureAtlas == nullptr)
					go->GetComponentManager()->GetComponent<Irenderer>()->textureAtlas = new TextureAtlas(spriteSize[0], spriteSize[1], pathtotext);
			}
			if (hascollision == 1) {
				go->GetComponentManager()->AddComponent<RectangleCollider2D>();
				go->GetComponentManager()->GetComponent<RectangleCollider2D>()->Translate(pos[0], pos[1]);
				go->GetComponentManager()->GetComponent<RectangleCollider2D>()->Enable = true;
				go->GetComponentManager()->GetComponent<RectangleCollider2D>()->SetOffset(offset[0], offset[1]);
				go->GetComponentManager()->GetComponent<RectangleCollider2D>()->IsTrigger = istrigger;
				go->GetComponentManager()->GetComponent<RectangleCollider2D>()->SetTag(tag);
				go->GetComponentManager()->GetComponent<Transform>()->Scale(scale[0], scale[1]);
			}
		}

	};

}

#endif