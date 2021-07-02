#pragma once

#include <iostream>
#include "Event.h"
#include <string>
#include "Core.h"

namespace Doom {

	class DOOM_API Listener {
	private:

		virtual void OnStart() {}
		virtual void OnUpdate() {}
		virtual void OnCollision(void* col) {}
		virtual void OnMove() {}
		virtual void OnRotate() {}
		virtual void OnScale() {}
		virtual void OnTranslate() {}
		virtual void OnWindowResize(void* props) {}
		virtual void OnMainThreadProcess(void* task) {}

	public:

		std::vector<int> m_RegisteredEvents;

		void HandleEvent(Event* e);

		virtual ~Listener() {}
	};

}