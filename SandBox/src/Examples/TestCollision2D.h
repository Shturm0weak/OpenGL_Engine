#pragma once

class Player : public GameObject {
public:
	Player() : GameObject("Player", 0, 10) {
		m_IsSerializable = false;
		GetComponentManager()->AddComponent<SpriteRenderer>();
		GetComponentManager()->AddComponent<RectangleCollider2D>();
		EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE, this);
		EventSystem::GetInstance()->RegisterClient(EventType::ONCOLLSION, this);
	}

	virtual void OnUpdate() override {
		GetComponent<SpriteRenderer>()->m_Color = COLORS::White;
		m_Transform->Move(glm::vec3(0, -9.8, 0));
		for (size_t i = 0; i < 1; i++)
		{
			GetComponentManager()->GetComponent<RectangleCollider2D>()->IsCollidedSAT();
		}
	}

	virtual void OnCollision(void* col) override {
		if (col != nullptr) {
			RectangleCollider2D* c = (RectangleCollider2D*)col;
			if (c->GetOwnerOfComponent()->m_Tag == "Target") {
				GetComponent<SpriteRenderer>()->m_Color = COLORS::Red;
			}
		}
	}

};

class TestCollision2D : public Application {
public:

	Player* p = nullptr;

	TestCollision2D(std::string name = "Test Collision2D", float x = 800, float y = 600, bool Vsync = false) : Application(name, TYPE_2D, x, y, Vsync) {
	}

	void OnStart() {
		GameObject* go = new GameObject("test", 0, 0);
		go->m_Tag = "Target";
		go->GetComponentManager()->AddComponent<SpriteRenderer>();
		go->GetComponentManager()->AddComponent<RectangleCollider2D>();
		go->m_Transform->Scale(10, 1, 0);
		p = new Player();
	}

	void OnUpdate() {
		//glm::vec2 mousePos = ViewPort::GetInstance()->GetMousePositionToWorldSpace();
		//p->m_Transform->Translate(glm::vec3(mousePos, 0));
	}

};
