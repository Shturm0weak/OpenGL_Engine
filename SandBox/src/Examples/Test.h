class TestComponent : public Component {
private:

	TestComponent() {
		EventSystem::GetInstance()->RegisterClient(EventType::ONSTART, this);
		EventSystem::GetInstance()->RegisterClient(EventType::ONUPDATE, this);
	}

	void OnStart() {

	}

	void OnUpdate() {

	}

public:

	static Component* Create() {
		return new TestComponent();
	}

};

class Test : public Application {
public:

	Test(std::string name = "Test", float x = 800, float y = 600, bool Vsync = false) : Application(name, TYPE_3D, x, y, Vsync) {
	}

	void OnStart() {

	}

	void OnUpdate() {
		
	}

};