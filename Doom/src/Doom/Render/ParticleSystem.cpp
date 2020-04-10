#include "../pch.h"
#include "ParticleSystem.h"

using namespace Doom;

ParticleSystem::ParticleSystem(float x, float y ,int amount,double lifeTime,float speed,float maxSize,float minSize,float radiusToSpawn, double minTimeToSpawn, double maxTimeToSpawn) :
	AmountOfParticles(amount),lifeTime(lifeTime),speedMultiplier(speed),maxSize(maxSize),minSize(minSize), radiusToSpawn(radiusToSpawn),minTimeToSpawn(minTimeToSpawn),maxTimeToSpawn(maxTimeToSpawn)
{
	SetName("ParticleSystem");
	position.x = x;
	position.y = y;
	Enable = false;
	particlesPool = new Particle[AmountOfParticles];
	std::random_device rd;
	std::mt19937 e2(rd());
	for (unsigned int i = 0; i < AmountOfParticles; i++)
	{
		particlesPool[i].SetOwner(this);
		AddChild((void*)(&particlesPool[i]));
		particlesPool[i].isParticle = true;
		particlesPool[i].Enable = false;
		particlesPool[i].SetTexture("src/Images/WhiteCircle.png");
		particlesPool[i].SetName("Particle");
		particlesPool[i].name->append("[" + std::to_string(i) + "]");
		Transform* tr = particlesPool[i].GetComponentManager()->GetComponent<Transform>();
		std::uniform_real_distribution<float> distributionraduis(-this->radiusToSpawn, this->radiusToSpawn);
		float radius = distributionraduis(e2);
		tr->Translate(position.x + radius,position.y + radius);
		std::uniform_real_distribution<float> distribution1(this->minSize, this->maxSize);
		float scale = distribution1(e2);
		tr->Scale(scale, scale);
		std::uniform_real_distribution<float> distribution(-1, 1);
		particlesPool[i].speeddiry = distribution(e2);
		particlesPool[i].speeddirx = distribution(e2);
		std::uniform_real_distribution<float> distribution2(this->minTimeToSpawn, this->maxTimeToSpawn);
		particlesPool[i].timeToSpawn = distribution2(e2);
		std::uniform_real_distribution<float> distributioncolor(60, 120);
		float colorGreen = distributioncolor(e2);
		color = glm::vec4(1, colorGreen / 255., 0, opacity);
		particlesPool[i].SetColor(color);
	}
}

void ParticleSystem::Play() {
	for (unsigned int i = 0; i < AmountOfParticles; i++)
	{
		if (particlesPool[i].timeToSpawn < timer && !particlesPool[i].isDead) {
			particlesPool[i].Enable = true;
		}
		if (particlesPool[i].lifeTime > this->lifeTime && !particlesPool[i].isDead) {
			particlesPool[i].Enable = false;
			particlesPool[i].lifeTime = 0;
			particlesPool[i].isDead = true;
			if (isLoop)
				Restart();
		}
		if (particlesPool[i].Enable) {
			Transform* tr = particlesPool[i].GetComponentManager()->GetComponent<Transform>();
			tr->Move(particlesPool[i].speeddirx * speedMultiplier, particlesPool[i].speeddiry * speedMultiplier - gravityAcceleration, 0);
			gravityAcceleration += (0.05 * DeltaTime::GetDeltaTime()) * gravity;
			particlesPool[i].lifeTime += DeltaTime::GetDeltaTime();
			if (isRotating)
				tr->Rotate(theta0,glm::vec3(0,0,1));
			if (isSmallerWithTime)
				tr->Scale(particlesPool[i].scaleValues[0]* (1 - (0.99 * DeltaTime::GetDeltaTime() * scaleMultiplier)), particlesPool[i].scaleValues[1] * (1 - (0.99 * DeltaTime::GetDeltaTime() * scaleMultiplier)));
		}
	}
	timer += DeltaTime::GetDeltaTime();
}

void ParticleSystem::Restart() {
	std::random_device rd;
	std::mt19937 e2(rd());
	for (unsigned int i = 0; i < AmountOfParticles; i++)
	{
		particlesPool[i].Enable = false;
		Transform* tr = particlesPool[i].GetComponentManager()->GetComponent<Transform>();
		std::uniform_real_distribution<float> distributionraduis(-this->radiusToSpawn, this->radiusToSpawn);
		float radius = distributionraduis(e2);
		tr->Translate(position.x + radius, position.y + radius);
		std::uniform_real_distribution<float> distribution1(this->minSize, this->maxSize);
		float scale = distribution1(e2);
		tr->Scale(scale, scale);
		std::uniform_real_distribution<float> distribution(-1, 1);
		particlesPool[i].speeddiry = distribution(e2);
		particlesPool[i].speeddirx = distribution(e2);
		std::uniform_real_distribution<float> distribution2(this->minTimeToSpawn, this->maxTimeToSpawn);
		particlesPool[i].timeToSpawn = distribution2(e2);
		particlesPool[i].isDead = false;
		particlesPool[i].lifeTime = 0;
	}
	timer = 0;
	gravityAcceleration = 0;
}

void ParticleSystem::ChangeOpacity(float opacity)
{
	color = glm::vec4(color[0], color[1], color[2], opacity);
	for (unsigned int i = 0; i < AmountOfParticles; i++)
	{
		particlesPool[i].SetColor(color);
	}
}

ParticleSystem::~ParticleSystem()
{
	delete[] particlesPool;
}
