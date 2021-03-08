#include "pch.h"
#include "ParticleEmitter.h"

using namespace Doom;

ParticleEmitter::ParticleEmitter()
{
	EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
}

void ParticleEmitter::Init(size_t amountOfParticles) {
	this->GetOwnerOfComponent()->m_IsSerializalbeChilds = false;
	m_ParticlesPool.resize(amountOfParticles);
	std::random_device rd;
	std::mt19937 e2(rd());
	glm::vec3 pos = this->GetOwnerOfComponent()->GetPosition();
	for (size_t i = 0; i < amountOfParticles; i++)
	{
		m_ParticlesPool[i].m_Particle = GameObject::Create("p" + std::to_string(i), pos.x, pos.y, pos.z);
		m_ParticlesPool[i].m_Particle->m_IsSerializable = false;
		m_ParticlesPool[i].m_Particle->SetOwner(this->GetOwnerOfComponent());
		Renderer3D* r3d = m_ParticlesPool[i].m_Particle->AddComponent<Renderer3D>();
		r3d->LoadMesh(MeshManager::GetInstance().GetMesh("sphere"));
		r3d->ChangeRenderTechnic(Renderer3D::RenderTechnic::Instancing);
		m_ParticlesPool[i].m_Particle->RemoveComponent<CubeCollider3D>();
		InitParticle(m_ParticlesPool[i]);
	}
}

void Doom::ParticleEmitter::Delete()
{
	{
		std::lock_guard<std::mutex> lg(m_Mtx);
		for (size_t i = 0; i < m_ParticlesPool.size(); i++)
		{
			World::GetInstance().DeleteObject(m_ParticlesPool[i].m_Particle->m_Id);
		}
	}
	delete this;
}

void ParticleEmitter::InitParticle(Particle3D& particle) {
	std::random_device rd;
	std::mt19937 e2(rd());
	particle.m_TimeLiving = 0.0;
	particle.m_IsSpawned = false;
	if (particle.m_Particle->GetComponent<Renderer3D>() == nullptr) return;
	particle.m_Particle->GetComponent<Renderer3D>()->m_Color = m_Color;
	particle.m_Particle->GetComponent<Renderer3D>()->m_Material.m_Ambient = 5.0f;
	std::uniform_real_distribution<float> radiusX(m_RadiusToSpawn[0].x, m_RadiusToSpawn[0].y);
	std::uniform_real_distribution<float> radiusY(m_RadiusToSpawn[1].x, m_RadiusToSpawn[1].y);
	std::uniform_real_distribution<float> radiusZ(m_RadiusToSpawn[2].x, m_RadiusToSpawn[2].y);
	particle.m_Particle->m_Transform->Translate(this->GetOwnerOfComponent()->GetPosition() + glm::vec3(radiusX(e2), radiusY(e2), radiusZ(e2)));
	std::uniform_real_distribution<double> timeToSpawn(m_TimeToSpawn.x, m_TimeToSpawn.y);
	particle.m_TimeToSpawn = timeToSpawn(e2);
	std::uniform_real_distribution<float> scale(m_Scale.x, m_Scale.y);
	particle.m_Scale = glm::vec3(scale(e2));
	particle.m_Particle->m_Transform->Scale(glm::vec3(0.0f));
	std::uniform_real_distribution<float> dirX(m_Dir[0].x, m_Dir[0].y);
	std::uniform_real_distribution<float> dirY(m_Dir[1].x, m_Dir[1].y);
	std::uniform_real_distribution<float> dirZ(m_Dir[2].x, m_Dir[2].y);
	particle.m_ParticleDir = glm::vec3(dirX(e2), dirY(e2), dirZ(e2));
}

void ParticleEmitter::Play()
{
	size_t pSize = m_ParticlesPool.size();

	for (size_t i = 0; i < pSize; i++)
	{
		if (m_ParticlesPool[i].m_TimeToSpawn > 0)
		{
			m_ParticlesPool[i].m_TimeToSpawn -= DeltaTime::s_Deltatime;
			continue;
		}
		else if (m_ParticlesPool[i].m_IsSpawned == false)
		{
			m_ParticlesPool[i].m_Particle->m_Transform->Scale(m_ParticlesPool[i].m_Scale);
			m_ParticlesPool[i].m_IsSpawned = true;
		}
		
		m_ParticlesPool[i].m_Particle->m_Transform->Move(m_ParticlesPool[i].m_ParticleDir * m_Speed);
		m_ParticlesPool[i].m_TimeLiving += DeltaTime::s_Deltatime;
		m_ParticlesPool[i].m_Particle->m_Transform->Scale(glm::vec3(m_ParticlesPool[i].m_Particle->GetScale().x * (1 - (0.99 * DeltaTime::s_Deltatime * m_Speed))));
		if (m_ParticlesPool[i].m_TimeLiving > m_MaxTimeToLive)
			InitParticle(m_ParticlesPool[i]);
	}
}

void ParticleEmitter::OnStart()
{

}

void ParticleEmitter::OnUpdate()
{
	ThreadPool::GetInstance().Enqueue([=]()
	{
		std::lock_guard<std::mutex> lg(m_Mtx);
		Play();
	});
}

Component* ParticleEmitter::Create()
{
	return new ParticleEmitter;
}