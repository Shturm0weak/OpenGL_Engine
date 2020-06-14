#pragma once

class RayCastTest : public Application {
public:
	std::vector<Ray*> rays;
	std::vector<Line*> lines;
	glm::vec2 pos = glm::vec2(0, 0);
	GameObject* coin = nullptr;

	RayCastTest(std::string name,float x ,float y , bool Vsync) : Application(name,x,y,Vsync){
	}

	virtual void OnStart() override {
		Renderer::Load("src/savedRayCastTest.txt");
		Window::GetCamera().Zoom(2.9);
		coin = new GameObject("Coin",5,5);
		Collision* col = coin->GetComponentManager()->AddComponent<Collision>();
		coin->GetComponentManager()->GetComponent<SpriteRenderer>()->SetTexture("src/Images/Bird.png");
		coin->GetComponentManager()->GetComponent<Transform>()->Scale(3, 3);
		col->SetTag("Coin");
		
		float angle = 0;
		for (unsigned int i = 0; i < 2000; i++)
		{
			rays.push_back(new Ray(glm::vec2(0, 0), glm::vec2(0, 1), 100));
			lines.push_back(new Line(glm::vec2(0, 0), glm::vec2(1, 1)));
			rays[i]->Rotate(angle);
			lines[i]->SetEndPoint(rays[i]->end.x, rays[i]->end.y);
			angle += 0.18f;
			lines[i]->color = COLORS::Gray;
			lines[i]->width = 1.f;
		}
	}

	virtual void OnUpdate() override {
		pos = glm::vec2(ViewPort::Instance()->GetMousePositionToWorldSpace().x, ViewPort::Instance()->GetMousePositionToWorldSpace().y);
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