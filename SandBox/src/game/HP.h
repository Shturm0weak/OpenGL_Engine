#pragma once
#include "pch.h"

using namespace Doom;

class HP : public Listener{
public:
	unsigned int count = 3;
	GameObject hearts[3];
	Texture* texture = new Texture("src/Images/Heart.png",1);
	float prevYPos = 0;
	bool isShake = false;
	bool firstphase[3] = { false,false,false };
	bool secondtphase[3] = { false,false,false };
	HP(float x, float y){
		prevYPos = y;
		EventSystem::Instance()->RegisterClient("OnUpdate", this);
		for (unsigned int i = 0; i < 3; i++)
		{
			hearts[i].SetName("HP");
			hearts[i].GetComponentManager()->GetComponent<Transform>()->Scale(2,2);
			hearts[i].Static = true;
			hearts[i].GetComponentManager()->GetComponent<Transform>()->Translate(x + i * (hearts[i].GetWidth() + 0.25),y);
			hearts[i].SetTexture(texture);
		}
	}

	void Shake() {
		isShake = true;
	}

	virtual void OnUpdate() override {
		if (isShake) {
			for (unsigned int i = 0; i < count - 1; i++)
			{
				Transform* tr = hearts[i].GetComponentManager()->GetComponent<Transform>();
				if (secondtphase[i]) {
					tr->Move(0, 40, 0);
					if (tr->position.y > prevYPos * 1.1) {
						secondtphase[i] = false;
						tr->Translate(tr->position.x, prevYPos);
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

				if (tr->position.y > prevYPos * 1.1) {
 					firstphase[i] = true;
				}
				else if (tr->position.y < prevYPos * 0.9) {
					firstphase[i] = false;
					secondtphase[i] = true;
				}


			}
		}
	}
};