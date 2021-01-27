#include "TestComponent.h"
#include "Core/EventSystem.h"

Doom::TestComponent::TestComponent()
{
	EventSystem::GetInstance()->RegisterClient(EventType::ONSTART, this);
	EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE, this);
}

void Doom::TestComponent::OnUpdate()
{
	std::cout << "Update\n";
}

void Doom::TestComponent::OnStart()
{
	std::cout << sizeof(Doom::Listener) << std::endl;
}

Doom::Component* Doom::TestComponent::Create()
{
	return new TestComponent();
}