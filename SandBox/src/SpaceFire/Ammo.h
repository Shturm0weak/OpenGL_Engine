#pragma once
class Ammo : public Component {
private:
	int ammo = 0;
	Sound* pickUp = new Sound("src/SpaceFire/Sounds/pickUp.wav");
public:
	void Init(int amount) {
		ammo = amount;
		m_OwnerOfCom->AddComponent<SpriteRenderer>();
		SoundManager::GetInstance().CreateSoundAsset("pickUp", pickUp);
	 }

	static Component* Create() {
		return new Ammo();
	}

	inline int GetAmmo() { return ammo; }
 };