#pragma once
#include "pch.h"
#include "HP.h"

using namespace Doom;

class Player : public GameObject {
public:
	RectangleCollider2D* col = nullptr;
	Transform* tr = nullptr;
	Animator* anim = nullptr;
	double speedx = 20.;
	double speedy = 0.;
	int prevKey = 65;
	int scores = 0;
	int missed = 0;
	int hp = 3;
	HP* HpBar = nullptr;
	bool isLanded = true;
	bool isDead = false;
	Player(const std::string name, float x, float y) :GameObject(name, x, y) {
		EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE,(GameObject*)this);
		EventSystem::GetInstance()->RegisterClient(EventType::ONSTART, (GameObject*)this);
		EventSystem::GetInstance()->RegisterClient(EventType::ONCOLLSION, (GameObject*)this);
		EventSystem::GetInstance()->RegisterClient(EventType::ONMISS, (GameObject*)this);
		col = GetComponentManager()->AddComponent<RectangleCollider2D>();
		col->SetTag("Player");
		tr = GetComponentManager()->GetComponent<Transform>();
		anim = GetComponentManager()->AddComponent<Animator>();
		anim->SetAnimation("src/Animations");
		tr->Scale(4,5);
		anim->isPlayingAnim = true;
		
	}

	virtual void OnMiss()override {
		missed++;
	}

	virtual void OnUpdate() override {
		if(hp <= 0){
			tr->Move(0, -10, 0);
			col->Enable = false;
			if (tr->GetPosition().y < -5) {
				isDead = true;
			}
		}
		else {
			if (missed >= 3) {
				missed -= 3;
				hp--;
				if (HpBar->hearts[hp]->Enable == true) {
					HpBar->hearts[hp]->Enable = false;
					HpBar->Shake();
					return;
				}
			}
			col->IsCollidedDIAGS();
			if (col->isCollided) {
				EventSystem::GetInstance()->SendEvent(EventType::ONCOLLSION, this, col->collidedObject);
				if (prevKey == Keycode::KEY_D) {
					auto iter = anim->animations.find(anim->GetAnimations()[0]);
					anim->PlayAnim(iter->second);
				}
				if (prevKey == Keycode::KEY_A) {
					auto iter = anim->animations.find(anim->GetAnimations()[1]);
					anim->PlayAnim(iter->second);
				}
			}
			Keys();
			col->IsCollidedDIAGS();
			if (!col->isCollided && tr->GetPosition().y > 20.) {
				if (prevKey == Keycode::KEY_D) {
					auto iter = anim->animations.find(anim->GetAnimations()[3]);
					anim->PlayAnim(iter->second);
				}
				if (prevKey == Keycode::KEY_A) {
					auto iter = anim->animations.find(anim->GetAnimations()[2]);
					anim->PlayAnim(iter->second);
				}
			}
			tr->Move(0, -5 + speedy, 0);
			if (!isLanded)
				if (speedy > 1) {
					speedy -= 20 * DeltaTime::deltatime;
					if (prevKey == Keycode::KEY_D) {
						auto iter = anim->animations.find(anim->GetAnimations()[7]);
						anim->PlayAnim(iter->second);
					}
					else if (prevKey == Keycode::KEY_A) {
						auto iter = anim->animations.find(anim->GetAnimations()[6]);
						anim->PlayAnim(iter->second);
					}
				}
				else if (speedy <= 1) {
					speedy -= 40 * DeltaTime::deltatime;
					if (prevKey == Keycode::KEY_D) {
						auto iter = anim->animations.find(anim->GetAnimations()[9]);
						anim->PlayAnim(iter->second);
					}
					else if (prevKey == Keycode::KEY_A) {
						auto iter = anim->animations.find(anim->GetAnimations()[8]);
						anim->PlayAnim(iter->second);
					}
				}
			if (tr->GetPosition().y < -18)
				Dead();
		}
	}

	virtual void OnCollision(void* _col) override {
		RectangleCollider2D* tempcol = static_cast<RectangleCollider2D*>(_col);
		if (tempcol->GetTag() == "Land") {
			isLanded = true;
			speedy = 0;
		}
	}

	void Keys() {
		tr->Move(0, -5, 0);
		if (Input::IsKeyDown(Keycode::SPACE) && isLanded) {
			speedy = 27;
			isLanded = false;
		}
		else if (Input::IsKeyDown(Keycode::KEY_D)) {
			tr->Move(speedx, 0, 0);
			if (isLanded){
				auto iter = anim->animations.find(anim->GetAnimations()[5]);
				anim->PlayAnim(iter->second);
			}
			prevKey = Keycode::KEY_D;
		}
		else if (Input::IsKeyDown(Keycode::KEY_A)) {
			tr->Move(-speedx, 0, 0);
			if (isLanded) {
				auto iter = anim->animations.find(anim->GetAnimations()[4]);
				anim->PlayAnim(iter->second);
			}
			prevKey = Keycode::KEY_A;
		}	
	}

	void Dead() {
		missed = 0;
		scores = 0;
		tr->Translate(0, 10);
		isDead = true;
	}
};