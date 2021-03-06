#include "pch.h"
#include "ShipEnemy.h"
#include "Bullet.h"
#include "Text/Gui.h"
#include "Ammo.h"

void ShipEnemy::OnCollision(void * _col)
{
	RectangleCollider2D* __col = static_cast<RectangleCollider2D*>(_col);
	if (__col->GetOwnerOfComponent()->m_Tag == "Bullet") {
		Bullet* bullet = static_cast<Doom::GameObject*>(__col->GetOwnerOfComponent())->GetComponent<Bullet>();
		hp -= bullet->damage;
		bullet->Death();
	}
}

void ShipEnemy::Init(std::string name, float x, float y)
{
	GetOwnerOfComponent()->m_Name = name;
	SoundManager::GetInstance().CreateSoundAsset("explosion",explosionSound);
	bulletsPlaceHolder = GameObject::Create("bulletsPlaceHolder", 0, 0);
	bulletsPlaceHolder->m_Enable = false;
	GetOwnerOfComponent()->AddChild((void*)bulletsPlaceHolder);
	GetOwnerOfComponent()->m_Listener = this;
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, (Component*)this);
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, (Component*)this);
	EventSystem::GetInstance().RegisterClient(EventType::ONCOLLISION, (Component*)this);
	sr = GetOwnerOfComponent()->AddComponent<SpriteRenderer>();
	col = GetOwnerOfComponent()->AddComponent<RectangleCollider2D>();
	tr = GetOwnerOfComponent()->GetComponent<Transform>();
	tr->Scale(5, 5);
	tr->RotateOnce(0,0,180);
	sr->m_Color = (COLORS::Red);
	sr->m_Texture = (texture);
	col->m_IsTrigger = false;
	for (unsigned int i = 0; i < amountOfBulletsInPool; i++)
	{
		bullets.push_back(GameObject::Create("Bullet" + std::to_string(i), GetOwnerOfComponent()->GetPosition().x, GetOwnerOfComponent()->GetPosition().y));
		Bullet* bullet = bullets[i]->AddComponent<Bullet>();
		bullet->Init("EnemyBullet", glm::vec3(0, -1, 0));
		bullet->isActive = false;
		bulletsPlaceHolder->AddChild((void*)bullets[i]);
		bullets[i]->m_Enable = false;
		bullet->col->m_Enable = false;
		bullet->damage = 10;
	}
	ps = new ParticleSystem(0, 0, 50, 1, 10, 1, 0.5, 0.5, 0, 0.2,nullptr);
	ps->SetOwner((void*)this);
}

void ShipEnemy::OnUpdate() {

	if (psPlay)
		ps->Play();

	if (GetOwnerOfComponent()->m_Enable == false)
		return;

	if (hp <= 0) {
		Death();
	}

	//ThreadPool::Instance()->enqueue([=] {
	col->IsCollidedSAT();// });
	Fire();

	if (GetOwnerOfComponent()->GetPosition().y < -30)
		Death();

	if (isDead == false)
		tr->Move(0, -3, 0);
}

void ShipEnemy::Spawn()
{
	hp = 100;
	GetOwnerOfComponent()->m_Enable = true;
	col->m_Enable = true;
	isDead = false;
	tr->Translate(GetOwnerOfComponent()->GetPosition().x, 20);
}

void ShipEnemy::Death()
{
	pl->kills++;
	GetOwnerOfComponent()->m_Enable = false;
	col->m_Enable = false;
	isDead = true;
	SoundManager::GetInstance().Play(explosionSound);
	ps->SetPosition(GetOwnerOfComponent()->GetPosition().x, GetOwnerOfComponent()->GetPosition().y);
	ps->Restart();
	psPlay = true;
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_int_distribution<int> distribution(0, 4);
	int chance = distribution(e2);
	std::uniform_int_distribution<int> distribution1(5, 40);
	int ammo = distribution1(e2);
	if (chance <= 1) {
		GameObject* a = GameObject::Create("AmmoPickUp", GetOwnerOfComponent()->GetPosition().x, GetOwnerOfComponent()->GetPosition().y);
		Ammo* aCom = a->AddComponent<Ammo>();
		aCom->Init(ammo);
		RectangleCollider2D* col = a->AddComponent<RectangleCollider2D>();
		(a->m_ComponentManager.GetComponent<SpriteRenderer>())->m_Texture = (Texture::Create("src/SpaceFire/Images/Ammo.png"));
		a->m_ComponentManager.GetComponent<Transform>()->Scale(2, 2);
		col->GetOwnerOfComponent()->m_Tag = ("Ammo");
		col->m_IsTrigger = true;
	}
}

void ShipEnemy::Fire() {
	if (timerFire > TimePerBullet) {
		timerFire = 0;
		if (usedBulletCounter == amountOfBulletsInPool)
			usedBulletCounter = 0;
		Bullet* bullet = bullets[usedBulletCounter]->GetComponent<Bullet>();
		bullet->SetMoveDirection(glm::vec3(0,-1,0));
		glm::vec3 pos = GetOwnerOfComponent()->GetPosition();
		bullets[usedBulletCounter]->m_Transform->Translate(pos.x, pos.y);
		bullets[usedBulletCounter]->m_Enable = true;
		bullet->col->m_Enable = true;
		bullet->isActive = true;
		bullet->lifeTimer = 0;
		usedBulletCounter++;
	}
	timerFire += DeltaTime::s_Deltatime;
}