#include "pch.h"
#include "ShipEnemy.h"
#include "Bullet.h"
#include "Text/Gui.h"
#include "Ammo.h"

void ShipEnemy::OnCollision(void * _col)
{
	RectangleCollider2D* __col = static_cast<RectangleCollider2D*>(_col);
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
	bulletsPlaceHolder->m_Enable = false;
	AddChild((void*)bulletsPlaceHolder);
	EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE, (GameObject*)this);
	EventSystem::GetInstance()->RegisterClient(EventType::ONSTART, (GameObject*)this);
	EventSystem::GetInstance()->RegisterClient(EventType::ONCOLLSION, (GameObject*)this);
	sr = GetComponentManager()->AddComponent<SpriteRenderer>();
	col = GetComponentManager()->AddComponent<RectangleCollider2D>();
	tr = GetComponentManager()->GetComponent<Transform>();
	tr->Scale(5, 5);
	tr->RotateOnce(0,0,180);
	sr->SetColor(COLORS::Red);
	sr->SetTexture(texture);
	col->IsTrigger = false;
	for (unsigned int i = 0; i < amountOfBulletsInPool; i++)
	{
		bullets.push_back(new Bullet("EnemyBullet", glm::vec3(0,-1,0), "Bullet" + std::to_string(i), GetPosition().x, GetPosition().y));
		bullets[i]->isActive = false;
		bullets[i]->SetOwner((void*)bulletsPlaceHolder);
		bulletsPlaceHolder->AddChild((void*)bullets[i]);
		bullets[i]->m_Enable = false;
		bullets[i]->col->Enable = false;
		bullets[i]->damage = 10;
	}
	ps = new ParticleSystem(0, 0, 50, 1, 10, 1, 0.5, 0.5, 0, 0.2,nullptr);
	ps->SetOwner((void*)this);
	AddChild((void*)ps);
}

void ShipEnemy::OnUpdate() {

	if (psPlay)
		ps->Play();

	if (m_Enable == false)
		return;

	if (hp <= 0) {
		Death();
	}

	//ThreadPool::Instance()->enqueue([=] {
	col->IsCollidedSAT();// });
	Fire();

	if (GetPosition().y < -30)
		Death();

	if (isDead == false)
		tr->Move(0, -3, 0);
}

void ShipEnemy::Spawn()
{
	hp = 100;
	m_Enable = true;
	col->Enable = true;
	isDead = false;
	tr->Translate(GetPosition().x, 20);
}

void ShipEnemy::Death()
{
	pl->kills++;
	m_Enable = false;
	col->Enable = false;
	isDead = true;
	SoundManager::Play(explosionSound);
	ps->SetPosition(GetPosition().x, GetPosition().y);
	ps->Restart();
	psPlay = true;
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_int_distribution<int> distribution(0, 4);
	int chance = distribution(e2);
	std::uniform_int_distribution<int> distribution1(5, 40);
	int ammo = distribution1(e2);
	if (chance <= 1) {
		Ammo* a = new Ammo("AmmoPickUp", GetPosition().x, GetPosition().y, ammo);
		RectangleCollider2D* col = a->GetComponentManager()->AddComponent<RectangleCollider2D>();
		(a->GetComponentManager()->GetComponent<SpriteRenderer>())->SetTexture(Texture::Create("src/SpaceFire/Images/Ammo.png"));
		a->GetComponentManager()->GetComponent<Transform>()->Scale(2, 2);
		col->SetTag("Ammo");
		col->IsTrigger = true;
	}
}

void ShipEnemy::Fire() {
	if (timerFire > TimePerBullet) {
		timerFire = 0;
		if (usedBulletCounter == amountOfBulletsInPool)
			usedBulletCounter = 0;
		bullets[usedBulletCounter]->SetMoveDirection(glm::vec3(0,-1,0));
		bullets[usedBulletCounter]->tr->Translate(GetPosition().x, GetPosition().y);
		bullets[usedBulletCounter]->m_Enable = true;
		bullets[usedBulletCounter]->col->Enable = true;
		bullets[usedBulletCounter]->isActive = true;
		bullets[usedBulletCounter]->lifeTimer = 0;
		usedBulletCounter++;
	}
	timerFire += DeltaTime::s_Deltatime;
}