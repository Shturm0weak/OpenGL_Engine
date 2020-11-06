#pragma once

#include "Audio/SoundManager.h"

using namespace Doom;

class Bullet;

enum Stages {
	ZERO = 1,
	FIRST = 2,
	SECOND = 3,
	THIRD = 4,
};

class ShipPlayer : public GameObject {
private:
	Stages currentStage = ZERO;

	void Fire(float xOffset = 0, float yOffset = 0);
	void ShipMovement();
	void ScreenBorders();
 
	unsigned int usedBulletCounter = 0;
	unsigned int amountOfBulletsInPool = 50;
public:
	Sound* fireSound = new Sound("src/SpaceFire/Sounds/fire.wav");
	int ammo = 100;
	unsigned int kills = 0;
	bool isDead = false;
	float hp = 100;
	double accelerationDecrease = 0.1;
	double MaxSpeed = 15.;
	double acceleration = 2.;
	glm::vec3 speed = glm::vec3(0,0,0);
	double fireRate = 10;
	double TimePerBullet = (100. / fireRate) / 100; //bullets per second
	double timerFire = 2;
	GameObject* bulletsPlaceHolder = nullptr;
	std::vector<Bullet*> bullets;
	glm::vec3 dir;
	SpriteRenderer* sr = nullptr;
	Transform* tr = nullptr;
	RectangleCollider2D* col = nullptr;
	Texture* texture = Texture::Create("src/SpaceFire/Images/SpaceShip.png");

	ShipPlayer(std::string name = "ShipPlayer",float x = 0,float y = 0);

	void Death();
	void Respawn();
	void OnStart();
	void OnUpdate();
	void OnCollision(void* _col);
};