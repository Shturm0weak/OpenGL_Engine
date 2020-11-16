#include "../pch.h"
#include "ParticleSystem.h"

using namespace Doom;

ParticleSystem::ParticleSystem(float x, float y ,int amount,double lifeTime,float speed,float maxSize,float minSize,float radiusToSpawn, double minTimeToSpawn, double maxTimeToSpawn,Texture* texture) :
	AmountOfParticles(amount),lifeTime(lifeTime),speedMultiplier(speed),maxSize(maxSize),minSize(minSize), radiusToSpawn(radiusToSpawn),minTimeToSpawn(minTimeToSpawn),maxTimeToSpawn(maxTimeToSpawn),texture(texture)
{
	name = "ParticleSystem";
	glm::vec2 position;
	position.x = x;
	position.y = y;
	Enable = false;
	particlesPool = new Particle[AmountOfParticles];
	std::random_device rd;
	std::mt19937 e2(rd());
	for (unsigned int i = 0; i < AmountOfParticles; i++)
	{
		particlesPool[i].Enable = false;
		particlesPool[i].texture = this->texture;
		std::uniform_real_distribution<float> distributionraduis(-this->radiusToSpawn, this->radiusToSpawn);
		float radius = distributionraduis(e2);
		particlesPool[i].x = position.x + radius;
		particlesPool[i].y = position.y + radius;
		particlesPool[i].pos = glm::translate(glm::mat4(1.f),glm::vec3(particlesPool[i].x, particlesPool[i].y,0));
		std::uniform_real_distribution<float> distribution1(this->minSize, this->maxSize);
		float scale = distribution1(e2);
		particlesPool[i].scaleX = scale; particlesPool[i].scaleY = scale;
		std::uniform_real_distribution<float> distribution(-1, 1);
		particlesPool[i].speeddiry = distribution(e2);
		particlesPool[i].speeddirx = distribution(e2);
		std::uniform_real_distribution<float> distribution2(this->minTimeToSpawn, this->maxTimeToSpawn);
		particlesPool[i].timeToSpawn = distribution2(e2);
		std::uniform_real_distribution<float> distributioncolor(60, 120);
		float colorGreen = distributioncolor(e2);
		color = glm::vec4(1, colorGreen / 255., 0, opacity);
		particlesPool[i].color = color;
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
			particlesPool[i].x += (particlesPool[i].speeddirx * speedMultiplier) * DeltaTime::deltatime;
			particlesPool[i].y += (particlesPool[i].speeddiry * speedMultiplier - gravityAcceleration) * DeltaTime::deltatime;

			particlesPool[i].pos = glm::translate(glm::mat4(1.f), glm::vec3(particlesPool[i].x, particlesPool[i].y, 0));

			gravityAcceleration += (0.05 * DeltaTime::GetDeltaTime()) * gravity;
			particlesPool[i].lifeTime += DeltaTime::GetDeltaTime();
			if (isRotating) {
				float angle = (-theta0 * (2 * 3.14159f) / 360.0f);
				particlesPool[i].view = glm::rotate(particlesPool[i].view, angle * DeltaTime::GetDeltaTime(), glm::vec3(0, 0, 1));
			}
			if (isSmallerWithTime)
			{
				particlesPool[i].scaleX = particlesPool[i].scaleX * (1 - (0.99 * DeltaTime::GetDeltaTime() * scaleMultiplier));
				particlesPool[i].scaleY = particlesPool[i].scaleY * (1 - (0.99 * DeltaTime::GetDeltaTime() * scaleMultiplier));
			}
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
		std::uniform_real_distribution<float> distributionraduis(-this->radiusToSpawn, this->radiusToSpawn);
		float radius = distributionraduis(e2);
		particlesPool[i].x = GetPosition().x + radius;
		particlesPool[i].y = GetPosition().y + radius;
		particlesPool[i].pos = glm::translate(glm::mat4(1.f), glm::vec3(particlesPool[i].x, particlesPool[i].y, 0));
		std::uniform_real_distribution<float> distribution1(this->minSize, this->maxSize);
		float scale = distribution1(e2);
		particlesPool[i].scaleX = scale; particlesPool[i].scaleY = scale;
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
		particlesPool[i].color = (color);
	}
}

ParticleSystem::~ParticleSystem()
{
	delete[] particlesPool;
}
