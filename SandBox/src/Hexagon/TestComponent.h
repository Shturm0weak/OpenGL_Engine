#include "ECS/Component.h"

namespace Doom {

	class TestComponent : public Component {
	public:
	

		TestComponent();

		static Component* Create();

		virtual void OnUpdate();

		virtual void OnStart();
	
	};

}