#include "pch.h"
#include "ShipEnemy.h"
#include "Bullet.h"
#include "Render/Gui.h"
#include "Ammo.h"

void ShipEnemy::OnCollision(void * _col)
{
	Collision* __col = static_cast<Collision*>(_col);
	if (__col->GetTag() == "Bullet") {
		Bullet* bullet = static_cast<Bullet*>(__col->GetOwnerOfComponent());
		hp -= bullet->damage;
		bullet->Death();
	}
}

ShipEnemy::ShipEnemy(std::string name, float x, float y) : GameObject(name, x, y)
{
	SoundManager::CreateSoundAsset("explosion",explosionSound);
	bulletsPlaceHolder = new GameObject("bulletsPlaceHolder", 0, 0);
	bulletsPlaceHolder->Enable = false;
	AddChild((void*)bulletsPlaceHolder);
	bulletsPlaceHolder->SetOwner((void*)this);
	EventSystem::GetInstance()->RegisterClient("OnUpdate", (GameObject*)this);
	EventSystem::GetInstance()->RegisterClient("OnStart", (GameObject*)this);
	EventSystem::GetInstance()->RegisterClient("OnCollision", (GameObject*)this);
	col = GetComponentManager()->AddComponent<Collision>();
	tr = GetComponentManager()->GetComponent<Transform>();
	sr = static_cast<SpriteRenderer*>(GetComponentManager()->GetComponent<Irenderer>());
	tr->Scale(5, 5);
	tr->RotateOnce(0,0,180);
	sr->SetColor(COLORS::Red);
	sr->SetTexture(texture);
	col->IsTrigger = false;
	for (unsigned int i = 0; i < amountOfBulletsInPool; i++)
	{
		bullets.push_back(new Bullet("EnemyBullet", glm::vec3(0,-1,0), "Bullet" + std::to_string(i), position.x, position.y));
		bullets[i]->isActive = false;
		bullets[i]->SetOwner((void*)bulletsPlaceHolder);
		bulletsPlaceHolder->AddChild((void*)bullets[i]);
		bullets[i]->Enable = false;
		bullets[i]->col->Enable = false;
		bullets[i]->damage = 10;
	}
	ps = new ParticleSystem(0, 0, 50, 1, 10, 1, 0.5, 0.5, 0, 0.2,nullptr);
	ps->SetOwner((void*)this);
	AddChild((void*)ps);
}

void ShipEnemy::OnUpdate() {

	Gui::GetInstance()->Begin();
	Gui::GetInstance()->Bar(0, 0, hp, 100, COLORS::Red, COLORS::DarkGray, 400, 50);
	Gui::GetInstance()->End();

	if (psPlay)
		ps->Play();

	if (Enable == false)
		return;

	if (hp <= 0) {
		Death();
	}

	//ThreadPool::Instance()->enqueue([=] {
	col->IsCollidedSAT();// });
	Fire();

	if (position.y < -30)
		Death();

	if (isDead == false)
		tr->Move(0, -3, 0);
}

void ShipEnemy::Spawn()
{
	hp = 100;
	Enable = true;
	col->Enable = true;
	isDead = false;
	tr->Translate(position.x, 20);
}

void ShipEnemy::Death()
{
	pl->kills++;
	Enable = false;
	col->Enable = false;
	isDead = true;
	SoundManager::Play(explosionSound);
	ps->SetPosition(position.x,position.y);
	ps->Restart();
	psPlay = true;
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_int_distribution<int> distribution(0, 4);
	int chance = distribution(e2);
	std::uniform_int_distribution<int> distribution1(5, 40);
	int ammo = distribution1(e2);
	if (chance <= 1) {
		Ammo* a = new Ammo("AmmoPickUp", position.x, position.y, ammo);
		static_cast<SpriteRenderer*>(a->GetComponentManager()->GetComponent<Irenderer>())->SetTexture("src/SpaceFire/Images/Ammo.png");
		a->GetComponentManager()->GetComponent<Transform>()->Scale(2, 2);
		Collision* col = a->GetComponentManager()->AddComponent<Collision>();
		col->SetTag("Ammo");
	}
}

void ShipEnemy::Fire() {
	if (timerFire > TimePerBullet) {
		timerFire = 0;
		if (usedBulletCounter == amountOfBulletsInPool)
			usedBulletCounter = 0;
		bullets[usedBulletCounter]->SetMoveDirection(glm::vec3(0,-1,0));
		bullets[usedBulletCounter]->tr->Translate(position.x, position.y);
		bullets[usedBulletCounter]->Enable = true;
		bullets[usedBulletCounter]->col->Enable = true;
		bullets[usedBulletCounter]->isActive = true;
		bullets[usedBulletCounter]->lifeTimer = 0;
		usedBulletCounter++;
	}
	timerFire += DeltaTime::deltatime;
}