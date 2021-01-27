#include "pch.h"
#include "ShipPlayer.h"
#include "Core/ViewPort.h"
#include "Bullet.h"
#include "Core/Timer.h"
#include "Ammo.h"

void ShipPlayer::ShipMovement()
{
	if(speed.x > 0)
		speed.x -= acceleration * accelerationDecrease;
	else if(speed.x < 0)
		speed.x += acceleration * accelerationDecrease;

	if (speed.y > 0)
		speed.y -= acceleration * accelerationDecrease;
	else if (speed.y < 0)
		speed.y += acceleration * accelerationDecrease;

	if (abs(speed.x) < 0.0001)
		speed.x = 0;
	if (abs(speed.y) < 0.0001)
		speed.y = 0;

	if (Input::IsKeyDown(Keycode::KEY_W)) {
		if(speed.y < MaxSpeed)
			speed.y += acceleration;
	}
	if (Input::IsKeyDown(Keycode::KEY_S)) {
		if (speed.y > -MaxSpeed)
			speed.y += -acceleration;
	}
	if (Input::IsKeyDown(Keycode::KEY_D)) {
		if (speed.x < MaxSpeed)
			speed.x += acceleration;
	}
	if (Input::IsKeyDown(Keycode::KEY_A)) {
		if (speed.x > -MaxSpeed)
			speed.x += -acceleration;
	}

	tr->Move(speed.x, speed.y, speed.z);
	dir = glm::vec3(ViewPort::GetInstance()->GetMousePositionToWorldSpace(), 0);
	glm::vec3 pos = GetOwnerOfComponent()->GetPosition();
	dir.x -= pos.x;
	dir.y -= pos.y;
	
	tr->RotateOnce(dir, glm::vec3(0, 0, 1));
}

void ShipPlayer::ScreenBorders()
{
	glm::vec3 pos = GetOwnerOfComponent()->GetPosition();
	double xBorder = (Window::GetCamera().GetAspectRatio() * Window::GetCamera().GetZoomLevel());
	if (pos.x > xBorder - sr->GetWidth() * 0.5) {
		tr->Translate(xBorder - sr->GetWidth() * 0.5, pos.y);
	}
	else if (pos.x < -xBorder + sr->GetWidth() * 0.5) {
		tr->Translate(-xBorder + sr->GetWidth() * 0.5, pos.y);
	}

	double yBorder = (1 * Window::GetCamera().GetZoomLevel());
	if (pos.y > yBorder - sr->GetHeight() * 0.5) {
		tr->Translate(pos.x, yBorder - sr->GetHeight() * 0.5);
	}
	else if (pos.y < -yBorder + sr->GetHeight() * 0.5) {
		tr->Translate(pos.x,-yBorder + sr->GetHeight() * 0.5);
	}
}

void ShipPlayer::Init(std::string name, float x, float y)
{
	GetOwnerOfComponent()->m_Transform->Translate(x, y);
	GetOwnerOfComponent()->m_Name = name;
	SoundManager::CreateSoundAsset("fire", fireSound);
	bulletsPlaceHolder = new GameObject("bulletsPlaceHolder", 0, 0);
	bulletsPlaceHolder->m_Enable = false;
	GetOwnerOfComponent()->AddChild((void*)bulletsPlaceHolder);
	EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE, (Component*)this);
	EventSystem::GetInstance()->RegisterClient(EventType::ONSTART, (Component*)this);
	EventSystem::GetInstance()->RegisterClient(EventType::ONCOLLISION, (Component*)this);
	GetOwnerOfComponent()->m_Listener = this;
	col = GetOwnerOfComponent()->AddComponent<RectangleCollider2D>();
	tr = GetOwnerOfComponent()->GetComponent<Transform>();
	sr = GetOwnerOfComponent()->AddComponent<SpriteRenderer>();
	tr->Scale(5, 5);
	sr->m_Texture = (texture);
	col->GetOwnerOfComponent()->m_Tag = ("Player");
	col->m_IsTrigger = true;
	for (unsigned int i = 0; i < amountOfBulletsInPool; i++)
	{
		bullets.push_back(new GameObject("Bullet" + std::to_string(i), GetOwnerOfComponent()->GetPosition().x, GetOwnerOfComponent()->GetPosition().y));
		Bullet* bullet = bullets[i]->AddComponent<Bullet>();
		bullet->Init("Bullet", glm::vec3(0, -1, 0));
		bullet->isActive = false;
		bulletsPlaceHolder->AddChild((void*)bullets[i]);
		bullets[i]->m_Enable = false;
		bullet->col->m_Enable = false;
		bullet->damage = 15;
	}
}

Component* ShipPlayer::Create()
{
	return new ShipPlayer();
}

void ShipPlayer::Death()
{
 	GetOwnerOfComponent()->m_Enable = false;
	col->m_Enable = false;
	isDead = true;
}

void ShipPlayer::Respawn()
{
	timerFire = 0;
	ammo = 100;
	hp = 100;
	GetOwnerOfComponent()->m_Enable = true;
	col->m_Enable = true;
	isDead = false;
	tr->Translate(0, 0);
	kills = 0;
}

void ShipPlayer::OnStart() {
	
}

void ShipPlayer::OnUpdate() {
	if (isDead == true)
		return;
	ShipMovement();
	ScreenBorders();
	//ThreadPool::Instance()->enqueue([=] {
		col->IsCollidedSAT();
	//});
	if (hp <= 0) {
		Death();
	}

	if (kills == 5) {
		currentStage = FIRST;
	}
	if (kills == 15) {
		currentStage = SECOND;
	}

	Fire();
}

void ShipPlayer::OnCollision(void * _col)
{
	RectangleCollider2D* __col = static_cast<RectangleCollider2D*>(_col);
	if (__col->GetOwnerOfComponent()->m_Tag == "EnemyBullet") {
		Bullet* bullet = static_cast<Doom::GameObject*>(__col->GetOwnerOfComponent())->GetComponent<Bullet>();
		hp -= bullet->damage;
		bullet->Death();
	}
	else if (__col->GetOwnerOfComponent()->m_Tag == "Ammo") {
		Ammo* a = static_cast<GameObject*>(__col->GetOwnerOfComponent())->GetComponent<Ammo>();
		ammo += a->GetAmmo();
		SoundManager::Play(SoundManager::GetSound("pickUp"));
		World::DeleteObject(a->GetOwnerOfComponent()->m_Id);
	}
}

void ShipPlayer::Fire(float xOffset,float yOffset) {
	if (ammo > 0 && timerFire > TimePerBullet && Input::IsMouseDown(GLFW_MOUSE_BUTTON_1)) {
		float worldVertexPositions[8];
		sr->GetTransformedVertices(worldVertexPositions);
		timerFire = 0;
		int amount = currentStage;
		if (currentStage != ZERO) {
			xOffset = worldVertexPositions[0] * 0.3f;
			yOffset = worldVertexPositions[1] * 0.3f;
		}
		for (size_t i = 0; i < amount; i++)
		{
			if (ammo <= 0)
				return;
			if (usedBulletCounter == amountOfBulletsInPool)
				usedBulletCounter = 0;
			Bullet* bullet = bullets[usedBulletCounter]->GetComponent<Bullet>();
			bullet->SetMoveDirection(dir);
			glm::vec3 pos = GetOwnerOfComponent()->GetPosition();
			bullets[usedBulletCounter]->m_Transform->Translate(pos.x + xOffset, pos.y + yOffset);
			bullets[usedBulletCounter]->m_Enable = true;
			bullet->col->m_Enable = true;
			bullet->isActive = true;
			bullet->lifeTimer = 0;
			usedBulletCounter++;
			ammo--;
			if (amount == 2) {
				xOffset = worldVertexPositions[2] * 0.3f;
				yOffset = worldVertexPositions[3] * 0.3f;
			}
			else if (amount == 3) {
				if (i == 0) {
					xOffset = 0;
					yOffset = 0;
				}
				else if (i == 1) {
					xOffset = worldVertexPositions[2] * 0.3f;
					yOffset = worldVertexPositions[3] * 0.3f;
				}
			}
		}
		SoundManager::Play(fireSound);
	}
	timerFire += DeltaTime::s_Deltatime;
}