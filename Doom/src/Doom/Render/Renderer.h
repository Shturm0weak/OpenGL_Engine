#pragma once
#ifndef RENDERER_H
#define RENDERER_H

#include <mutex>
#include "../Components/Collision.h"

namespace Doom {

	class DOOM_API Renderer {
	public:
		static int DrawCalls;
		static bool isReadyToRenderFirstThread;
		static bool isReadyToRenderSecondThread;
		static bool isReadyToRenderThirdThread;
		static void DeleteObject(int id);
		static void DeleteAll();
		static void SubmitGameObjects();
		static void Render();
		static void CalculateMVPforAllObjects();
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
		static void RenderShutDown();

		static const char** GetItems();

		static GameObject* CreateGameObject();

	private:
		static bool ObjectCollided(std::vector<glm::vec2>& p,int i);
		static std::vector<unsigned int> ObjectsWithNoOwner;
		static std::vector<unsigned int> ObjectsWithOwner;
		static std::vector <Collision*> collision2d;
		static int obj_id;
		static int col_id;
		static std::vector <GameObject*> objects2d;
		static std::mutex mtx;
		static std::condition_variable condVar;

		friend class Renderer2DLayer;
		friend class GameObject;
		friend class SpriteRenderer;
		friend class Editor;
		friend class ComponentManager;
		friend class Ray;
		friend class Collision;

		template<typename T>
		static void LoadObj(bool enable,std::string name, std::string pathtotext, float angle, float color[4],
			float scale[3], double pos[2], bool hascollision, float* offset, int* axes, bool istrigger, std::string tag,float* uvs,bool isSprite,float* spriteSize) {
			T* go = new T(name, pos[0], pos[1]);
			go->Enable = enable;
			go->GetComponentManager()->GetComponent<Transform>()->Scale(scale[0], scale[1]);
			go->GetComponentManager()->GetComponent<Transform>()->RotateOnce(angle, glm::vec3(axes[0], axes[1], axes[2]),true);
			go->GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[2] = uvs[0];
			go->GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[3] = uvs[1];
			go->GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[6] = uvs[2];
			go->GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[7] = uvs[3];
			go->GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[10] = uvs[4];
			go->GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[11] = uvs[5];
			go->GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[14] = uvs[6];
			go->GetComponentManager()->GetComponent<SpriteRenderer>()->mesh2D[15] = uvs[7];
			go->GetComponentManager()->GetComponent<SpriteRenderer>()->SetColor(glm::vec4(color[0], color[1], color[2], color[3]));
			if(pathtotext != "None")
				go->GetComponentManager()->GetComponent<SpriteRenderer>()->SetTexture(pathtotext);
			if (isSprite) {
				for (unsigned int i = 0; i < TextureAtlas::textureAtlases.size(); i++)
				{
					if (TextureAtlas::textureAtlases[i]->m_texture->GetFilePath() == pathtotext) {
						go->GetComponentManager()->GetComponent<SpriteRenderer>()->textureAtlas = TextureAtlas::textureAtlases[i];
					}
				}
				if(go->GetComponentManager()->GetComponent<SpriteRenderer>()->textureAtlas == nullptr)
					go->GetComponentManager()->GetComponent<SpriteRenderer>()->textureAtlas = new TextureAtlas(spriteSize[0], spriteSize[1], pathtotext);
			}
			if (hascollision == 1) {
				go->GetComponentManager()->AddComponent<Collision>();
				go->GetComponentManager()->GetComponent<Collision>()->Translate(pos[0], pos[1]);
				go->GetComponentManager()->GetComponent<Collision>()->Enable = true;
				go->GetComponentManager()->GetComponent<Collision>()->SetOffset(offset[0], offset[1]);
				go->GetComponentManager()->GetComponent<Collision>()->IsTrigger = istrigger;
				go->GetComponentManager()->GetComponent<Collision>()->SetTag(tag);
				go->GetComponentManager()->GetComponent<Transform>()->Scale(scale[0], scale[1]);
			}
		}

	};

}

#endif