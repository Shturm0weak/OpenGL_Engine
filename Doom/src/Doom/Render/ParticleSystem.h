#pragma once
#include <random>

namespace Doom {

	struct DOOM_API Particle {
		float x = 0; float y = 0;
		glm::mat4 pos = glm::mat4(1.f);
		glm::mat4 view = glm::mat4(1.f);
		glm::vec4 color = glm::vec4(1.f);
		double lifeTime = 0;
		float speeddirx = 0;
		float speeddiry = 0;
		double timeToSpawn = 0;
		bool isDead = false;
		bool Enable = true;
		Texture* texture = nullptr;
		float scaleX = 1;
		float scaleY = 1;
		static std::vector<Particle*> particles;
		Particle() {
			particles.push_back(this);
		}

		friend class Renderer;
	};

	class DOOM_API ParticleSystem : private GameObject{
	public:
		glm::vec4 color;
		unsigned int AmountOfParticles = 10;
		float maxSize = 0.5f, minSize = 0.1f;
		double lifeTime = 0.1;
		bool isSmallerWithTime = true;
		bool isLoop = false;
		bool isRotating = false;
		float radiusToSpawn = 0;
		float scaleMultiplier = 2;
		float speedMultiplier = 1;
		float minTimeToSpawn = 0;
		float maxTimeToSpawn = 1;
		float theta0 = 1; //Rotation speed
		float gravity = 1;
		Texture* texture = nullptr;

		ParticleSystem(float x, float y, int amount, double lifeTime, float speed, float maxSize, float minSize, float radiusToSpawn, double minTimeToSpawn, double maxTimeToSpawn, Texture* texture = nullptr);

		~ParticleSystem();

		void Play();

		void Restart();

		void SetPosition(float x, float y) { GetComponentManager()->GetComponent<Transform>()->Translate(x, y); }

		void ChangeOpacity(float opacity); //from 0 to 1
	private:
		float opacity = 1;
		float gravityAcceleration = 0;
		double timer = 0;
		

		Particle* particlesPool = nullptr;
	};

}