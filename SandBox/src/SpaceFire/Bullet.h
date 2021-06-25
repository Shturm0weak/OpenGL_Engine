#pragma once

class Bullet : public Component {
public:
	float damage = 15.f;
	double lifeTimer = 0;
	double maxTimeToLive = 2;
	Transform* tr = nullptr;
	RectangleCollider2D* col = nullptr;
	SpriteRenderer* sr = nullptr;
	bool isActive = false;
	glm::vec3 moveDir;
	float speed = 40.f;
	Texture* texture = Texture::Create("src/SpaceFire/Images/WhiteCircle.png");
	int useless = 0;

	static Component* Create() {
		return new Bullet();
	}

	void Init(std::string tag, glm::vec3 moveDir, std::string name = "Bullet") {
		EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
		EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
		EventSystem::GetInstance().RegisterClient(EventType::ONCOLLISION, this);
		col = m_OwnerOfCom->AddComponent<RectangleCollider2D>();
		tr = m_OwnerOfCom->GetComponent<Transform>();
		sr = m_OwnerOfCom->AddComponent<SpriteRenderer>();
		col->m_OwnerOfCom->m_Tag = (tag);
		this->moveDir = glm::vec3(moveDir * (1.f / sqrtf(moveDir.x * moveDir.x + moveDir.y * moveDir.y + moveDir.z * moveDir.z)));
		sr->m_Texture = (texture);
		sr->m_Color = (COLORS::Orange);
		tr->Scale(0.5, 0.5);
		col->m_IsTrigger = true;
	}

	virtual void OnUpdate() override {
		if (isActive) {
			tr->Move(moveDir.x * speed, moveDir.y * speed, moveDir.z * speed);
			lifeTimer += DeltaTime::s_Deltatime;
		}
		if (lifeTimer > maxTimeToLive) {
			Death();
		}
	}

	void SetMoveDirection(glm::vec3 moveDir) {
		this->moveDir = glm::vec3(moveDir * (1.f / sqrtf(moveDir.x * moveDir.x + moveDir.y * moveDir.y + moveDir.z * moveDir.z)));
	}

	void Death() {
		isActive = false;
		col->m_Enable = false;
		m_OwnerOfCom->m_Enable = false;
	}
};