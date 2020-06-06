#pragma once
#include "Render/ParticleSystem.h"

using namespace Doom;

class Bomb : public GameObject {

	Collision* col = nullptr;
	Transform* tr = nullptr;
	Texture* whiteCircle = new Texture("src/Images/WhiteCircle.png");
	ParticleSystem* particle = new ParticleSystem(0, 0, 100, 2.5, 10, 2, 1, 0.5, 0, 0.2,whiteCircle);
	bool particlePlay = false;
public:
	Bomb(const std::string name = "Bomb", float x = 0, float y = 0) :GameObject(name, x, y) {
		EventSystem::Instance()->RegisterClient("OnUpdate", (GameObject*)this);
		EventSystem::Instance()->RegisterClient("OnCollision", (GameObject*)this);
		col = GetComponentManager()->AddComponent<Collision>();
		tr = GetComponentManager()->GetComponent<Transform>();
		*Renderer2DLayer::type = "GameObject";
		tr->Scale(2, 2);
		col->IsTrigger = true;
		col->SetTag("Bomb");
		Randomize();
		SetTexture("src/Images/bomb.png");
		particle->ChangeOpacity(0.2f);
		particle->gravity = 2.f;
	}

	void OnUpdate() {
		col->IsCollidedDIAGS();
		if (col->isCollided) {
			EventSystem::Instance()->SendEvent("OnCollision", this, col->collidedObject);
		}
		tr->Move(0, -5, 0);
		tr->Rotate(1,glm::vec3(0,0,1));
		if (tr->position.y <= -5) {
			Randomize();
		}
		if (particlePlay) {
			particle->Play();
		}
	}

	void OnCollision(void* _col) {
		Collision* col = static_cast<Collision*>(_col);
		if (col->GetTag() == "Player") {
			Enable = false;
			this->col->Enable = false;
			Player* player = dynamic_cast<Player*>(&col->GetOwnerOfComponent());
			player->missed += 3;
			player = nullptr;
			particle->SetPosition(position.x, position.y);
			particlePlay = true;
			particle->Restart();
		}
		else if (col->GetTag() == "Coin") {
			Randomize();
		}
		else if (col->GetTag() == "Land") {
			Enable = false;
			this->col->Enable = false;
			particle->SetPosition(position.x, position.y);
			particlePlay = true;
			particle->Restart();
		}
		col = nullptr;
	}

	void Randomize() {
		Enable = true;
		this->col->Enable = true;
		std::random_device rd;
		std::mt19937 e2(rd());
		std::uniform_real_distribution<float> distribution(-20.f, 20.f);
		float x = distribution(e2);
		std::uniform_real_distribution<float> distribution1(0.f, 5.f);
		float y = distribution1(e2);
		tr->Translate(x, 30 + y);
	}

};