#pragma once

class RayCastTest : public Application {
public:
	std::vector<Ray2D*> rays;
	std::vector<Line*> lines;
	glm::vec2 pos = glm::vec2(0, 0);
	GameObject* coin = nullptr;

	RayCastTest(std::string name,float x ,float y , bool Vsync) : Application(name,TYPE_2D,x,y,Vsync){
	}

	virtual void OnStart() override {
		Window::GetCamera().Zoom(28);
		coin = new GameObject("Coin",5,5);
		RectangleCollider2D* col = coin->GetComponentManager()->AddComponent<RectangleCollider2D>();
		static_cast<SpriteRenderer*>(coin->GetComponentManager()->GetComponent<Irenderer>())->SetTexture(Texture::Create("src/Images/Bird.png"));
		coin->GetComponentManager()->GetComponent<Transform>()->Scale(3, 3);
		col->SetTag("Coin");
		
		float angle = 0;
		for (unsigned int i = 0; i < 2000; i++)
		{
			rays.push_back(new Ray2D(glm::vec2(0, 0), glm::vec2(0, 1), 100));
			lines.push_back(new Line(glm::vec3(0, 0, 0), glm::vec3(1, 1, 1)));
			rays[i]->Rotate(angle);
			lines[i]->SetEndPoint(rays[i]->end.x, rays[i]->end.y);
			angle += 0.18f;
			lines[i]->color = COLORS::Gray;
			lines[i]->width = 1.f;
		}
	}

	virtual void OnUpdate() override {
		pos = glm::vec2(ViewPort::GetInstance()->GetMousePositionToWorldSpace().x, ViewPort::GetInstance()->GetMousePositionToWorldSpace().y);
		coin->Enable = false;
		for (unsigned int i = 0; i < 2000; i++)
		{
			lines[i]->SetStartPoint(pos.x,pos.y);
			Hit hit;
			rays[i]->SetStart(pos);
			if (rays[i]->Raycast(hit, 100, pos, rays[i]->direction, rays[i]->ignoreMask)) {
				if (hit.Object->GetTag() == "Coin")
					coin->Enable = true;
				lines[i]->SetEndPoint(hit.point.x, hit.point.y);
			}
			else
				lines[i]->SetEndPoint(rays[i]->end.x, rays[i]->end.y);
		}
	}

	virtual void OnClose() override {
		//Renderer::Save("src/savedRayCastTest.txt");
	}
};