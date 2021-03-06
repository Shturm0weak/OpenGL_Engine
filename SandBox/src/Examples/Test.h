class TestComponent : public Component {
private:

	TestComponent() {
		EventSystem::GetInstance().RegisterClient(EventType::ONSTART, this);
		EventSystem::GetInstance().RegisterClient(EventType::ONUPDATE, this);
	}

	void OnStart() {

	}

	void OnUpdate() {
		m_OwnerOfCom->m_Transform->Move(1,1,1);
	}

public:

	static Component* Create() {
		return new TestComponent();
	}

};

class Test : public Application {
public:

	GameObject* player = nullptr;

	Test(std::string name = "Test", float x = 800, float y = 600, bool Vsync = false) : Application(name, TYPE_3D, x, y, Vsync) {}

	void OnStart() {
		player = GameObject::Create("Player");
		player->AddComponent<TestComponent>();
		player->AddComponent<Renderer3D>()->LoadMesh(MeshManager::GetInstance().GetMesh("cube"));
	}

	void OnUpdate() {
		
	}

};