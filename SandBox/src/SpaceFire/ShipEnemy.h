#pragma once
#include "ShipPlayer.h"

using namespace Doom;

class Bullet;

class ShipEnemy : public GameObject {
private:
	unsigned int usedBulletCounter = 0;
	unsigned int amountOfBulletsInPool = 10;

	void Death();
	void Fire();
public:
	ShipPlayer* pl = nullptr;

	void Spawn();

	bool isDead = false;
	double fireRate = 2;
	double TimePerBullet = (100. / fireRate) / 100; //bullets per second
	double timerFire = 2;
	GameObject* bulletsPlaceHolder = nullptr;
	std::vector<Bullet*> bullets;

	float hp = 100;

	Transform* tr = nullptr;
	Collision* col = nullptr;
	Texture* texture = new Texture("src/SpaceFire/Images/SpaceShip.png");

	void OnCollision(void* _col);
	void OnUpdate();

	ShipEnemy(std::string name = "ShipEnemy", float x = 0, float y = 0);
	~ShipEnemy();
};