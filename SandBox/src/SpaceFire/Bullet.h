#pragma once

class Bullet : public GameObject {
public:
	float damage = 15.f;
	double lifeTimer = 0;
	double maxTimeToLive = 5;
	Transform* tr = nullptr;
	Collision* col = nullptr;
	SpriteRenderer* sr = nullptr;
	bool isActive = false;
	glm::vec3 moveDir;
	float speed = 40.f;
	Texture* texture = new Texture("src/SpaceFire/Images/WhiteCircle.png");

	Bullet(std::string tag,glm::vec3 moveDir,std::string name = "Bullet", float x = 0, float y = 0) : GameObject(name, x, y) {
		EventSystem::Instance()->RegisterClient("OnUpdate", (GameObject*)this);
		EventSystem::Instance()->RegisterClient("OnStart", (GameObject*)this);
		EventSystem::Instance()->RegisterClient("OnCollision", (GameObject*)this);
		col = GetComponentManager()->AddComponent<Collision>();
		tr = GetComponentManager()->GetComponent<Transform>();
		sr = GetComponentManager()->GetComponent<SpriteRenderer>();
		col->SetTag(tag);
		this->moveDir = glm::vec3(moveDir * (1.f / sqrtf(moveDir.x * moveDir.x + moveDir.y * moveDir.y + moveDir.z * moveDir.z)));
		sr->SetTexture(texture);
		sr->SetColor(COLORS::Orange);
		tr->Scale(0.5, 0.5);
		col->IsTrigger = true;
	}

	virtual void OnUpdate() override {
		if (isActive) {
			tr->Move(moveDir.x * speed, moveDir.y * speed, moveDir.z * speed);
			lifeTimer += DeltaTime::deltatime;
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
		col->Enable = false;
		Enable = false;
	}
};