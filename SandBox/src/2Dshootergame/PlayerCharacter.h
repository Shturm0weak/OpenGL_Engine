#pragma once

using namespace Doom;

class PlayerCharacter : public GameObject{
public:
	RectangleCollider2D* col = nullptr;
	Transform* tr = nullptr;
	Animator* anim = nullptr;

	std::mutex mtx;
	int count = 0;
	Ray2D* checkGround = nullptr;
	Line* test1 = nullptr;
	Line* test2 = nullptr;
	Line* test3 = nullptr;

	float x = 0;

	float speed = 5.f;
	float Hp = 100.f;
	float jump = 0.f;
	unsigned int turnSide = 0;

	Ray2D* rayfire = nullptr;
	Line* line = nullptr;

	bool isLanded = false;

	float fireRate = 0.01f;
	float fireRateTimer = 0.2f;

	unsigned int animIndex = 0;
	unsigned int currentAnimIndex = 0;

	Texture* muzzleFlash = new Texture("src/Images/MuzzleFlash.png");

	GameObject* muzzleFlashObj = nullptr;
	glm::vec2 flashOffset = glm::vec2(1.1f,-0.1f);

	PlayerCharacter(const std::string name, float x, float y);
	~PlayerCharacter();

	void OnStart();
	void OnUpdate();

	void OnCollision(void* _col);

	void PlayerMovement();
};