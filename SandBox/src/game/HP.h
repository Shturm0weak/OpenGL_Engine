#pragma once
#include "pch.h"

using namespace Doom;

class HP : public Listener{
public:
	unsigned int count = 3;
	std::vector <GameObject*> hearts;
	Texture* texture = Texture::Create("src/Images/Heart.png",1);
	float prevYPos = 0;
	bool isShake = false;
	bool firstphase[3] = { false,false,false };
	bool secondtphase[3] = { false,false,false };
	float y = 0;
	HP(float x, float y){
		prevYPos = y;
		EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE, this);
		for (unsigned int i = 0; i < 3; i++)
		{
			hearts.push_back(new GameObject());
			SpriteRenderer* sr = static_cast<SpriteRenderer*>(hearts[i]->GetComponentManager()->GetComponent<Irenderer>());
			hearts[i]->name = ("HP");
			hearts[i]->GetComponentManager()->GetComponent<Transform>()->Scale(2,2);
			hearts[i]->GetComponentManager()->GetComponent<Transform>()->Translate(x + (i * (sr->GetWidth() + 1)),y);
			sr->Setlayer(i);
			this->y = y;
			sr->SetTexture(texture);
		}
	}
	
	void Shake() {
		isShake = true;
	}

	virtual void OnUpdate() override {
		if (isShake) {
			for (unsigned int i = 0; i < count - 1; i++)
			{
				Transform* tr = hearts[i]->GetComponentManager()->GetComponent<Transform>();
				if (secondtphase[i]) {
					tr->Move(0, 40, 0);
					if (tr->GetPosition().y > prevYPos * 1.1) {
						secondtphase[i] = false;
						tr->Translate(tr->GetPosition().x, prevYPos);
						isShake = false;
					}
					continue;
				}
				if (!firstphase[i]) {
					tr->Move(0, 40, 0);
				}
				else if (firstphase[i]) {
					tr->Move(0, -40, 0);
				}

				if (tr->GetPosition().y > prevYPos * 1.1) {
 					firstphase[i] = true;
				}
				else if (tr->GetPosition().y < prevYPos * 0.9) {
					firstphase[i] = false;
					secondtphase[i] = true;
				}
			}
		}
	}
};