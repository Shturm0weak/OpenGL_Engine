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
	line->width = 2.f;
}

PlayerCharacter::~PlayerCharacter()
{
}

void PlayerCharacter::OnStart()
{
}

void PlayerCharacter::OnUpdate()
{
	fireRateTimer += DeltaTime::deltatime;
	muzzleFlashObj->GetComponentManager()->GetComponent<Transform>()->Translate(position.x + flashOffset.x, position.y + flashOffset.y);
	PlayerMovement();
	col->IsCollidedDIAGS();
	if (col->isCollided) {
		EventSystem::Instance()->SendEvent("OnCollision", this, col->collidedObject);
	}
	tr->Move(0, -5, 0);
	col->IsCollidedDIAGS();
}

void PlayerCharacter::OnCollision(void* _col) {
	Collision* tempcol = static_cast<Collision*>(_col);
	if (tempcol->GetTag() == "Land") {
		isLanded = true;
	}
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
		isLanded = false;
		jump = 20;
	}
	if (!isLanded) {
		animIndex = 2;
		if(jump > 0){
			jump -= 20 * DeltaTime::deltatime;
		}
	}
	auto iter = anim->animations.find(anim->GetAnimations()[animIndex]);
	anim->PlayAnim(iter->second);
}
