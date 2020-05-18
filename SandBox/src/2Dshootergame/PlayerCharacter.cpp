#include "pch.h"
#include "Render/Line.h"
#include "Core/Ray.h"
#include "PlayerCharacter.h"
#include "Core/Sound.h"

PlayerCharacter::PlayerCharacter(const std::string name, float x, float y) :GameObject(name, x, y)
{
	EventSystem::Instance()->RegisterClient("OnUpdate", (GameObject*)this);
	EventSystem::Instance()->RegisterClient("OnStart", (GameObject*)this);
	EventSystem::Instance()->RegisterClient("OnCollision", (GameObject*)this);
	col = GetComponentManager()->AddComponent<Collision>();
	col->SetTag("Player");
	tr = GetComponentManager()->GetComponent<Transform>();
	*Renderer2DLayer::type = "Player";
	anim = GetComponentManager()->AddComponent<Animator>();
	anim->SetAnimation("src/ShooterAnimation");
	tr->Scale(3, 3);
	col->Scale(3,3);
	anim->isPlayingAnim = true;
	muzzleFlashObj = new GameObject("MuzzleFlashObj",position.x,position.x);
	muzzleFlashObj->SetOwner(this);
	AddChild(muzzleFlashObj);
	muzzleFlashObj->SetTexture(muzzleFlash);
	muzzleFlashObj->GetComponentManager()->GetComponent<Transform>()->Scale(0.4f, 0.4f);
	SetObjectType("Player");
	muzzleFlashObj->SetObjectType("MuzzleFlash");
	rayfire = new Ray(glm::vec2(position.x,position.y),glm::vec2(1,0),15);
	line = new Line(glm::vec2(0,0), glm::vec2(0, 0),15);
	test1 = new Line(glm::vec2(0, 0), glm::vec2(0, 0), 15);
	test2 = new Line(glm::vec2(0, 0), glm::vec2(0, 0), 15);
	test3 = new Line(glm::vec2(0, 0), glm::vec2(0, 0), 15);
	line->width = 2.f;
	checkGround = new Ray(glm::vec2(position.x, position.y), glm::vec2(0, -1), 100);
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
	muzzleFlashObj->GetComponentManager()->GetComponent<Transform>()->Translate(position.x + flashOffset.x, position.y + flashOffset.y);
	PlayerMovement();
	//ThreadPool::Instance()->enqueue([=] {
	//	mtx.lock();
		tr->Move(0, -9.8, 0);
		col->IsCollidedSAT();
	//	mtx.unlock();
	//});
		Hit hit;
		count = 0;
		glm::vec2 resultV;
		if (checkGround->Raycast(hit, 150, glm::vec2(this->WorldVertexPositions[0] + position.x,position.y), glm::vec2(0, -1), checkGround->ignoreMask)) {
			if (hit.Object->GetTag() == "Land") {
 				x = ((abs(this->WorldVertexPositions[1] + position.y)) - abs(hit.point.y));
				test1->SetStartPoint(checkGround->start.x, checkGround->start.y);
				test1->SetEndPoint(hit.point.x, hit.point.y);
				if (x < 0.05 && x > -0.15) {
					isLanded = true;
					count++;
					
				}
				
			}
		}

		
		if (checkGround->Raycast(hit, 150, glm::vec2(this->WorldVertexPositions[2] + position.x, position.y), glm::vec2(0, -1), checkGround->ignoreMask)) {
			if (hit.Object->GetTag() == "Land") {

				x = ((abs(this->WorldVertexPositions[3] + position.y)) - abs(hit.point.y));
				test2->SetStartPoint(checkGround->start.x, checkGround->start.y);
				test2->SetEndPoint(hit.point.x, hit.point.y);
				if (x < 0.05 && x > -0.15) {
					isLanded = true;
					count++;
					
				}
				
			}
		}

		if (checkGround->Raycast(hit, 150, glm::vec2(position.x, position.y), glm::vec2(0, -1), checkGround->ignoreMask)) {
			if (hit.Object->GetTag() == "Land") {
				x = ((abs(this->WorldVertexPositions[3] + position.y)) - abs(hit.point.y));
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

}

void PlayerCharacter::OnCollision(void* _col) {

}

void PlayerCharacter::PlayerMovement()
{
	animIndex = 1;
	tr->Move(0, -5 + jump, 0);
	if (fireRateTimer > fireRate && Input::IsMousePressed(GLFW_MOUSE_BUTTON_1)) {
		//Sound::Play(L"src/Sounds/GunFire.wav");
		Hit hit;
		glm::vec2 direction;
		if (turnSide == 0)
			direction = glm::vec2(1,0);
		else if(turnSide == 1)
			direction = glm::vec2(-1, 0);
		rayfire->Raycast(hit, 15, glm::vec2(position.x,position.y) , direction , rayfire->ignoreMask);
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
	if (Input::IsKeyPressed(Keycode::KEY_D)) {
		tr->Move(speed, 0,0);
		animIndex = 3;
		OriginalUvs();
		muzzleFlashObj->OriginalUvs();
		flashOffset.x = 1.1f;
		turnSide = 0;
	}
	if (Input::IsKeyPressed(Keycode::KEY_A)) {
		tr->Move(-speed, 0,0);
		animIndex = 3;
		muzzleFlashObj->ReversedUvs();
		ReversedUvs();
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
