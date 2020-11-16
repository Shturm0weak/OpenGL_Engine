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
	anim->isPlayingAnim = true;
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
	checkGround->ignoreMask.push_back("Player");
}

void PlayerCharacter::OnUpdate()
{
	fireRateTimer += DeltaTime::deltatime;
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
		if (checkGround->Raycast(hit, 150, glm::vec2(sr->WorldVertexPositions[0] + GetPosition().x, GetPosition().y), glm::vec2(0, -1), checkGround->ignoreMask)) {
			if (hit.Object->GetTag() == "Land") {
 				x = ((abs(sr->WorldVertexPositions[1] + GetPosition().y)) - abs(hit.point.y));
				test1->SetStartPoint(checkGround->start.x, checkGround->start.y);
				test1->SetEndPoint(hit.point.x, hit.point.y);
				if (x < 0.05 && x > -0.15) {
					isLanded = true;
					count++;
					
				}
				
			}
		}

		
		if (checkGround->Raycast(hit, 150, glm::vec2(sr->WorldVertexPositions[2] + GetPosition().x, GetPosition().y), glm::vec2(0, -1), checkGround->ignoreMask)) {
			if (hit.Object->GetTag() == "Land") {

				x = ((abs(sr->WorldVertexPositions[3] + GetPosition().y)) - abs(hit.point.y));
				test2->SetStartPoint(checkGround->start.x, checkGround->start.y);
				test2->SetEndPoint(hit.point.x, hit.point.y);
				if (x < 0.05 && x > -0.15) {
					isLanded = true;
					count++;
					
				}
				
			}
		}

		if (checkGround->Raycast(hit, 150, glm::vec2(GetPosition().x, GetPosition().y), glm::vec2(0, -1), checkGround->ignoreMask)) {
			if (hit.Object->GetTag() == "Land") {
				x = ((abs(sr->WorldVertexPositions[3] + GetPosition().y)) - abs(hit.point.y));
				test3->SetStartPoint(checkGround->start.x, checkGround->start.y);
				test3->SetEndPoint(hit.point.x, hit.point.y);
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
		rayfire->Raycast(hit, 15, glm::vec2(GetPosition().x, GetPosition().y) , direction , rayfire->ignoreMask);
		fireRateTimer = 0.0f;
		muzzleFlashObj->Enable = true;
		line->SetStartPoint(rayfire->start.x,rayfire->start.y);
		line->SetEndPoint(rayfire->end.x, rayfire->end.y);
		line->Enable = true;
	}
	else {
		line->Enable = false;
		muzzleFlashObj->Enable = false;
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
			jump -= 30 * DeltaTime::deltatime;
		}
	}
	auto iter = anim->animations.find(anim->GetAnimations()[animIndex]);
	anim->PlayAnim(iter->second);
}
