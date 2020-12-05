#include "pch.h"
#include "Objects/Line.h"
#include "Rays/Ray2D.h"
#include "PlayerCharacter.h"
#include "Audio/Sound.h"

PlayerCharacter::PlayerCharacter(const std::string name, float x, float y) :GameObject(name, x, y)
{
	EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE, (GameObject*)this);
	EventSystem::GetInstance()->RegisterClient(EventType::ONSTART, (GameObject*)this);
	EventSystem::GetInstance()->RegisterClient(EventType::ONCOLLSION, (GameObject*)this);
	col = GetComponentManager()->AddComponent<RectangleCollider2D>();
	col->SetTag("Player");
	tr = GetComponentManager()->GetComponent<Transform>();
	anim = GetComponentManager()->AddComponent<Animator>();
	anim->SetAnimation("src/ShooterAnimation");
	tr->Scale(3, 3);
	col->Scale(3,3);
	anim->m_IsPlayingAnim = true;
	muzzleFlashObj = new GameObject("MuzzleFlashObj",GetPosition().x, GetPosition().x);
	muzzleFlashObj->SetOwner(this);
	AddChild(muzzleFlashObj);
	static_cast<SpriteRenderer*>(muzzleFlashObj->GetComponentManager()->GetComponent<Irenderer>())->SetTexture(muzzleFlash);
	muzzleFlashObj->GetComponentManager()->GetComponent<Transform>()->Scale(0.4f, 0.4f);
	rayfire = new Ray2D(glm::vec2(GetPosition().x, GetPosition().y),glm::vec2(1,0),15);
	line = new Line(glm::vec3(0), glm::vec3(0),15);
	test1 = new Line(glm::vec3(0), glm::vec3(0), 15);
	test2 = new Line(glm::vec3(0), glm::vec3(0), 15);
	test3 = new Line(glm::vec3(0), glm::vec3(0), 15);
	line->width = 2.f;
	checkGround = new Ray2D(glm::vec2(GetPosition().x, GetPosition().y), glm::vec2(0, -1), 100);
}

PlayerCharacter::~PlayerCharacter()
{
}

void PlayerCharacter::OnStart()
{
	checkGround->m_IgnoreMask.push_back("Player");
}

void PlayerCharacter::OnUpdate()
{
	fireRateTimer += DeltaTime::m_Deltatime;
	muzzleFlashObj->GetComponentManager()->GetComponent<Transform>()->Translate(GetPosition().x + flashOffset.x, GetPosition().y + flashOffset.y);
	PlayerMovement();
	SpriteRenderer* sr = static_cast<SpriteRenderer*>(GetComponentManager()->GetComponent<Irenderer>());
	//ThreadPool::Instance()->enqueue([=] {
		//mtx.lock();
		tr->Move(0, -9.8, 0);
		col->IsCollidedSAT();
		Hit hit;
		count = 0;
		glm::vec2 resultV;
		if (checkGround->Raycast(hit, 150, glm::vec2(sr->m_WorldVertexPositions[0] + GetPosition().x, GetPosition().y), glm::vec2(0, -1), checkGround->m_IgnoreMask)) {
			if (hit.m_Object->GetTag() == "Land") {
 				x = ((abs(sr->m_WorldVertexPositions[1] + GetPosition().y)) - abs(hit.m_Point.y));
				test1->SetStartPoint(checkGround->m_Start.x, checkGround->m_Start.y);
				test1->SetEndPoint(hit.m_Point.x, hit.m_Point.y);
				if (x < 0.05 && x > -0.15) {
					isLanded = true;
					count++;
					
				}
				
			}
		}

		
		if (checkGround->Raycast(hit, 150, glm::vec2(sr->m_WorldVertexPositions[2] + GetPosition().x, GetPosition().y), glm::vec2(0, -1), checkGround->m_IgnoreMask)) {
			if (hit.m_Object->GetTag() == "Land") {

				x = ((abs(sr->m_WorldVertexPositions[3] + GetPosition().y)) - abs(hit.m_Point.y));
				test2->SetStartPoint(checkGround->m_Start.x, checkGround->m_Start.y);
				test2->SetEndPoint(hit.m_Point.x, hit.m_Point.y);
				if (x < 0.05 && x > -0.15) {
					isLanded = true;
					count++;
					
				}
				
			}
		}

		if (checkGround->Raycast(hit, 150, glm::vec2(GetPosition().x, GetPosition().y), glm::vec2(0, -1), checkGround->m_IgnoreMask)) {
			if (hit.m_Object->GetTag() == "Land") {
				x = ((abs(sr->m_WorldVertexPositions[3] + GetPosition().y)) - abs(hit.m_Point.y));
				test3->SetStartPoint(checkGround->m_Start.x, checkGround->m_Start.y);
				test3->SetEndPoint(hit.m_Point.x, hit.m_Point.y);
				if (x < 0.05 && x > -0.15) {
					isLanded = true;
					count++;
					
				}
			
			}
		}

		if(count <= 0)
			isLanded = false;
		//mtx.unlock();
	//});
}

void PlayerCharacter::OnCollision(void* _col) {

}

void PlayerCharacter::PlayerMovement()
{
	SpriteRenderer* sr = static_cast<SpriteRenderer*>(GetComponentManager()->GetComponent<Irenderer>());
	SpriteRenderer* mSr = static_cast<SpriteRenderer*>(muzzleFlashObj->GetComponentManager()->GetComponent<Irenderer>());
	animIndex = 1;
	tr->Move(0, -5 + jump, 0);
	if (fireRateTimer > fireRate && Input::IsMouseDown(Keycode::MOUSE_BUTTON_1)) {
		//Sound::Play(L"src/Sounds/GunFire.wav");
		Hit hit;
		glm::vec2 direction;
		if (turnSide == 0)
			direction = glm::vec2(1,0);
		else if(turnSide == 1)
			direction = glm::vec2(-1, 0);
		rayfire->Raycast(hit, 15, glm::vec2(GetPosition().x, GetPosition().y) , direction , rayfire->m_IgnoreMask);
		fireRateTimer = 0.0f;
		muzzleFlashObj->m_Enable = true;
		line->SetStartPoint(rayfire->m_Start.x,rayfire->m_Start.y);
		line->SetEndPoint(rayfire->m_End.x, rayfire->m_End.y);
		line->m_Enable = true;
	}
	else {
		line->m_Enable = false;
		muzzleFlashObj->m_Enable = false;
	}
	if (Input::IsKeyDown(Keycode::KEY_D)) {
		tr->Move(speed, 0,0);
		animIndex = 3;
		sr->OriginalUvs();
		mSr->OriginalUvs();
		flashOffset.x = 1.1f;
		turnSide = 0;
	}
	if (Input::IsKeyDown(Keycode::KEY_A)) {
		tr->Move(-speed, 0,0);
		animIndex = 3;
		sr->ReversedUvs();
		mSr->ReversedUvs();
		flashOffset.x = -1.1f;
		turnSide = 1;
	}
	if (Input::IsKeyPressed(Keycode::SPACE) && isLanded) {
		jump = 30;
		isLanded = false;
	}
	if (!isLanded) {
		animIndex = 2;
		if(jump > 0){
			jump -= 30 * DeltaTime::m_Deltatime;
		}
	}
	auto iter = anim->animations.find(anim->GetAnimations()[animIndex]);
	anim->PlayAnim(iter->second);
}
