#pragma once
#include "pch.h"
#include <random>

using namespace Doom;

class Coin: public GameObject{
public:
	Collision* col = nullptr;
	Transform* tr = nullptr;
	bool playanim = false;
	bool secondphase = false;
	float prevscale[2];

	Coin(const std::string name = "Coin", float x = 0, float y = 30) :GameObject(name, x, y) {
		EventSystem::Instance()->RegisterClient("OnUpdate", (GameObject*)this);
		EventSystem::Instance()->RegisterClient("OnCollision", (GameObject*)this);
		col = GetComponentManager()->AddComponent<Collision>();
		tr = GetComponentManager()->GetComponent<Transform>();
		*Renderer2DLayer::type = "GameObject";
		tr->Scale(2, 2);
		col->IsTrigger = true;
		col->SetTag("Coin");
		prevscale[0] = scaleValues[0];
		prevscale[1] = scaleValues[1];
	}

	virtual void OnUpdate() override {

		if (playanim) {

			if (!secondphase) {
				tr->Scale(scaleValues[0] * 1.04f, scaleValues[1] * 1.04f);
				if (scaleValues[0] > prevscale[0] * 1.5f)
					secondphase = true;
			}
			else if (secondphase) {
				tr->Scale(scaleValues[0] * 0.95f, scaleValues[1] * 0.95f);
			}

			if (scaleValues[0] < prevscale[0] * 0.5f) {
				auto rand = std::bind(&Coin::Randomize, this);
				rand();
				playanim = false;
				secondphase = false;
				tr->Scale(prevscale[0],prevscale[1]);
			}
		}
		col->IsCollidedDIAGS();
		if (col->isCollided) {
			EventSystem::Instance()->SendEvent("OnCollision", this, col->collidedObject);
		}
		tr->Move(0,-5,0);
		if (tr->position.y <= -5) {
			Randomize();
			EventSystem::Instance()->SendEvent("OnMiss", nullptr);
		}
		
	}

	virtual void OnCollision(void* _col)override {
		Collision* col = static_cast<Collision*>(_col);
		if (col->GetTag() == "Player") {
			//Enable = false;
			this->col->Enable = false;
			Player* player = dynamic_cast<Player*>(&col->GetOwnerOfComponent());
			player->scores++;
			player = nullptr;
			playanim = true;
		}
		else if(col->GetTag() == "Coin"){
			Randomize();
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