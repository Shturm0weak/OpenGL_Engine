#include "pch.h"
#include "ShipEnemy.h"
#include "Bullet.h"
#include "Text/Gui.h"
#include "Ammo.h"

void ShipEnemy::OnCollision(void * _col)
{
	RectangleCollider2D* __col = static_cast<RectangleCollider2D*>(_col);
	if (__col->m_OwnerOfCom->m_Tag == "Bullet") {
		Bullet* bullet = static_cast<Doom::GameObject*>(__col->m_OwnerOfCom)->GetComponent<Bullet>();
		hp -= bullet->damage;
		bullet->Death();
	}
}

void ShipEnemy::Init(std::string name, float x, float y)
{
	m_OwnerOfCom->m_Name = name;
	SoundManager::GetInstance().CreateSoundAsset("explosion",explosionSound);
	bulletsPlaceHolder = GameObject::Create("bulletsPlaceHolder", 0, 0);
	bulletsPlaceHolder->m_Enable = false;
	m_OwnerOfCom->AddChild((void*)bulletsPlaceHolder);
	m_OwnerOfCom->m_Listener = this;
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, (Component*)this);
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, (Component*)this);
	EventSystem::GetInstance().RegisterClient(EventType::ONCOLLISION, (Component*)this);
	sr = m_OwnerOfCom->AddComponent<SpriteRenderer>();
	col = m_OwnerOfCom->AddComponent<RectangleCollider2D>();
	tr = m_OwnerOfCom->GetComponent<Transform>();
	tr->Scale(5, 5);
	tr->RotateOnce(0,0,180);
	sr->m_Color = (COLORS::Red);
	sr->m_Texture = (texture);
	col->m_IsTrigger = false;
	for (unsigned int i = 0; i < amountOfBulletsInPool; i++)
	{
		bullets.push_back(GameObject::Create("Bullet" + std::to_string(i), m_OwnerOfCom->GetPosition().x, m_OwnerOfCom->GetPosition().y));
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

	if (m_OwnerOfCom->m_Enable == false)
		return;

	if (hp <= 0) {
		Death();
	}

	//ThreadPool::Instance()->enqueue([=] {
	col->IsCollidedSAT();// });
	Fire();

	if (m_OwnerOfCom->GetPosition().y < -30)
		Death();

	if (isDead == false)
		tr->Move(0, -3, 0);
}

void ShipEnemy::Spawn()
{
	hp = 100;
	m_OwnerOfCom->m_Enable = true;
	col->m_Enable = true;
	isDead = false;
	tr->Translate(m_OwnerOfCom->GetPosition().x, 20);
}

void ShipEnemy::Death()
{
	pl->kills++;
	m_OwnerOfCom->m_Enable = false;
	col->m_Enable = false;
	isDead = true;
	SoundManager::GetInstance().Play(explosionSound);
	ps->SetPosition(m_OwnerOfCom->GetPosition().x, m_OwnerOfCom->GetPosition().y);
	ps->Restart();
	psPlay = true;
	std::random_device rd;
	std::mt19937 e2(rd());
	std::uniform_int_distribution<int> distribution(0, 4);
	int chance = distribution(e2);
	std::uniform_int_distribution<int> distribution1(5, 40);
	int ammo = distribution1(e2);
	if (chance <= 1) {
		GameObject* a = GameObject::Create("AmmoPickUp", m_OwnerOfCom->GetPosition().x, m_OwnerOfCom->GetPosition().y);
		Ammo* aCom = a->AddComponent<Ammo>();
		aCom->Init(ammo);
		RectangleCollider2D* col = a->AddComponent<RectangleCollider2D>();
		(a->m_ComponentManager.GetComponent<SpriteRenderer>())->m_Texture = (Texture::Create("src/SpaceFire/Images/Ammo.png"));
		a->m_ComponentManager.GetComponent<Transform>()->Scale(2, 2);
		col->m_OwnerOfCom->m_Tag = ("Ammo");
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
		glm::vec3 pos = m_OwnerOfCom->GetPosition();
		bullets[usedBulletCounter]->m_Transform.Translate(pos.x, pos.y);
		bullets[usedBulletCounter]->m_Enable = true;
		bullet->col->m_Enable = true;
		bullet->isActive = true;
		bullet->lifeTimer = 0;
		usedBulletCounter++;
	}
	timerFire += DeltaTime::s_Deltatime;
}