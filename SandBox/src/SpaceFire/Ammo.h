#pragma once
class Ammo : public GameObject {
private:
	int ammo = 0;
	Sound* pickUp = new Sound("src/SpaceFire/Sounds/pickUp.wav");
public:
	Ammo(std::string name,float x, float y,int amount) : GameObject(name,x,y) {
		ammo = amount;
		GetComponentManager()->AddComponent<SpriteRenderer>();
		SoundManager::CreateSoundAsset("pickUp", pickUp);
	 }

	inline int GetAmmo() { return ammo; }
 };