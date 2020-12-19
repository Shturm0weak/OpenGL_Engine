#pragma once

#include "Core/SceneSerializer.h"
#include "Rays/Ray2D.h"

class RayCastTest : public Application {
public:
	std::vector<Ray2D*> rays;
	std::vector<Line*> lines;
	glm::vec2 pos = glm::vec2(0, 0);
	GameObject* coin = nullptr;

	RayCastTest(std::string name = "RayCast", float x = 800, float y = 600, bool Vsync = false) : Application(name, TYPE_2D, x, y, Vsync){
	}

	virtual void OnStart() override {
		SceneSerializer::DeSerialize("src/Scenes/RayCastTest.yaml");
		Renderer::s_Exposure = 2.0;
		Renderer::s_Brightness = 0.0;
		Renderer::s_BloomEffect = true;
		coin = new GameObject("Bird",5,5);
		coin->m_IsSerializable = false;
		RectangleCollider2D* col = coin->GetComponentManager()->AddComponent<RectangleCollider2D>();
		coin->GetComponentManager()->AddComponent<SpriteRenderer>()->SetTexture(Texture::Create("src/Images/Bird.png"));
		coin->GetComponentManager()->GetComponent<Transform>()->Scale(3, 3);
		col->SetTag("Bird");
		
		float angle = 0;
		for (unsigned int i = 0; i < 2000; i++)
		{
			rays.push_back(new Ray2D(glm::vec2(0, 0), glm::vec2(0, 1), 100));
			lines.push_back(new Line(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
			rays[i]->Rotate(angle);
			lines[i]->SetEndPoint(rays[i]->m_End.x, rays[i]->m_End.y);
			angle += 0.18f;
			lines[i]->m_Color = COLORS::Gray;
			lines[i]->s_Width = 1.f;
		}
	}

	virtual void OnUpdate() override {
		pos = ViewPort::GetInstance()->GetMousePositionToWorldSpace();
		coin->m_Enable = false;
		for (unsigned int i = 0; i < 2000; i++)
		{
			lines[i]->SetStartPoint(pos.x,pos.y);
			Ray2D::Hit hit;
			rays[i]->SetStart(pos);
			if (rays[i]->Raycast(hit, 100, pos, rays[i]->m_Dir, rays[i]->m_IgnoreMask)) {
				if (hit.m_Object->GetTag() == "Bird")
					coin->m_Enable = true;
				lines[i]->SetEndPoint(hit.m_Point.x, hit.m_Point.y);
			}
			else
				lines[i]->SetEndPoint(rays[i]->m_End.x, rays[i]->m_End.y);
		}
	}

	virtual void OnClose() override {
		SceneSerializer::Serialize("src/Scenes/RayCastTest.yaml");
	}
};