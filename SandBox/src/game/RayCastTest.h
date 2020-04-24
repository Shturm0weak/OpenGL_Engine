#pragma once

class RayCastTest : public Application {
public:
	std::vector<Ray*> rays;
	std::vector<Line*> lines;
	glm::vec2 pos = glm::vec2(0, 0);
	GameObject* cursor = nullptr;
	GameObject* coin = nullptr;


	virtual void OnStart() override {
		Renderer::Load("src/savedRayCastTest.txt");
		coin = new GameObject("Coin",5,5);
		Collision* col = coin->GetComponentManager()->AddComponent<Collision>();
		coin->SetTexture("src/Images/Bird.png");
		coin->GetComponentManager()->GetComponent<Transform>()->Scale(3, 3);
		col->SetTag("Coin");
		cursor = new GameObject("Cursor",0,0);
		cursor->SetTexture("src/Images/WhiteCircle.png");
		cursor->SetColor(COLORS::Red);
		float angle = 0;
		for (unsigned int i = 0; i < 1000; i++)
		{
			rays.push_back(new Ray(glm::vec2(0, 0), glm::vec2(0, 1), 100));
			lines.push_back(new Line(glm::vec2(0, 0), glm::vec2(1, 1)));
			rays[i]->Rotate(angle);
			lines[i]->SetEndPoint(rays[i]->end.x, rays[i]->end.y);
			angle += 0.36f;
			lines[i]->color = COLORS::Yellow;
			lines[i]->width = 1.f;
		}
	}

	virtual void OnUpdate() override {
		cursor->GetComponentManager()->GetComponent<Transform>()->Translate(pos.x, pos.y);
		pos = glm::vec2(Window::GetMousePositionToWorldSpace().x, Window::GetMousePositionToWorldSpace().y);
		coin->Enable = false;
		for (unsigned int i = 0; i < 1000; i++)
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
		
	}
};