#pragma once

#include <random>

namespace Doom {

	struct Particle3D {
		glm::vec3 m_ParticleDir = glm::vec3(0.0f);
		glm::vec3 m_Scale = glm::vec3(0.0f);
		GameObject* m_Particle = nullptr;
		double m_TimeLiving = 0.0;
		double m_TimeToSpawn = 0.0;
		bool m_IsSpawned = false;
	};

	class DOOM_API ParticleEmitter : public Component {
	public:

		bool m_IsInitialized = false;
		glm::vec4 m_Color = COLORS::White;
		glm::vec2 m_Scale = glm::vec2(0.0f);
		glm::vec2 m_TimeToSpawn = glm::vec2(0.0f);;
		std::vector<glm::vec2> m_Dir = { glm::vec2(0.0f), glm::vec2(0.0f), glm::vec2(0.0f) };
		std::vector<glm::vec2> m_RadiusToSpawn = { glm::vec2(0.0f), glm::vec2(0.0f), glm::vec2(0.0f) };
		std::vector<Particle3D> m_ParticlesPool;
		float m_MaxTimeToLive = 1.0f;
		float m_Speed = 1.0f;
		std::mutex m_Mtx;
		ParticleEmitter();

		void Init(size_t amountOfParticles = 10);
		void Play();
		void OnStart();
		void OnUpdate();

		void Copy(const ParticleEmitter& rhs);
		void operator=(const ParticleEmitter& rhs);
		static Component* Create();
		virtual void Delete() override;

		void InitParticle(Particle3D& particle, std::mt19937& e2);
	};

}