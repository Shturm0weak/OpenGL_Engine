#pragma once

#define G 9.8

class Player : public Component {
public:

	Animator* anim = nullptr;

	float m_Speed = 10.0f;
	float m_CurrentJumpSpeed = 0.0f;
	float m_MaxJumpSpeed = 15.0f;
	bool m_IsLanded = false;
	
	static Component* Create() 
	{
		return new Player();
	}

	void OnStart()
	{
		GetOwnerOfComponent()->m_IsSerializable = false;
		GetOwnerOfComponent()->AddComponent<SpriteRenderer>();
		GetOwnerOfComponent()->AddComponent<RectangleCollider2D>();
		anim = GetOwnerOfComponent()->AddComponent<Animator>();
		anim->SetAnimation("src/Animations");
		anim->m_IsPlayingAnim = true;
		EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
		EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
	}

	void Movement() {
		GameObject* owner = GetOwnerOfComponent();
		if (Input::IsKeyDown(Keycode::KEY_W)) {
			owner->m_Transform.Move(0, m_Speed, 0);
		}
		else if (Input::IsKeyDown(Keycode::KEY_S)) {
			owner->m_Transform.Move(0, -m_Speed, 0);
		}
		if (Input::IsKeyDown(Keycode::KEY_D)) {
			anim->PlayAnim(anim->animations.at("src/Animations\\RunRight"));
			owner->m_Transform.Move(m_Speed, 0, 0);
		}
		else if (Input::IsKeyDown(Keycode::KEY_A)) {
			anim->PlayAnim(anim->animations.at("src/Animations\\RunLeft"));
			owner->m_Transform.Move(-m_Speed, 0, 0);
		}
		else {
			anim->PlayAnim(anim->animations.at("src/Animations\\IdleLeft"));
		}
		if (Input::IsKeyPressed(Keycode::SPACE) && m_IsLanded) {
			anim->PlayAnim(anim->animations.at("src/Animations\\JumpRight"));
			m_CurrentJumpSpeed = m_MaxJumpSpeed;
			m_IsLanded = false;
		}

		m_CurrentJumpSpeed -= G * DeltaTime::s_Deltatime;
		m_CurrentJumpSpeed = glm::clamp(m_CurrentJumpSpeed, 0.0f, m_MaxJumpSpeed);
		owner->m_Transform.Move(glm::vec3(0, -G + m_CurrentJumpSpeed, 0));
	}

	void OnUpdate() {
		Movement();
		GetComponent<SpriteRenderer>()->m_Color = COLORS::White;
		GetOwnerOfComponent()->GetComponent<RectangleCollider2D>()->IsCollidedSAT();
		Window::GetInstance().GetCamera().SetPosition(GetOwnerOfComponent()->GetPosition());
	}

	void OnCollision(void* col) {
		if (col != nullptr) {
			RectangleCollider2D* c = (RectangleCollider2D*)col;
			if (c->GetOwnerOfComponent()->m_Tag == "Land") {
				m_IsLanded = true;
			}
		}
	}

};

class TestCollision2D : public Application {
public:

	GameObject* p = nullptr;

	TestCollision2D(std::string name = "Test Collision2D", float x = 800, float y = 600, bool Vsync = false) : Application(name, TYPE_2D, x, y, Vsync) {
	}

	void OnStart() {
		SceneSerializer::DeSerialize("src/Scenes/TestCollision.yaml");
		p = GameObject::Create();
		p->AddComponent<Player>();
	}

	void OnUpdate() {
		//glm::vec2 mousePos = ViewPort::GetInstance()->GetMousePositionToWorldSpace();
		//p->m_Transform.Translate(glm::vec3(mousePos, 0));
	}

};
