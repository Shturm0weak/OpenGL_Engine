#pragma once
#ifndef LISTENER_H_
#define LISTENER_H_
#include <iostream>
#include "Event.h"
#include <string>
#include "Core.h"

namespace Doom {

	class DOOM_API Listener {
	private:

		virtual void OnUpdate() {}
		virtual void OnCollision(void* col) {}
		virtual void OnMove() {}
		virtual void OnRotate() {}
		virtual void OnStart() {}
		virtual void OnScale() {}
		virtual void OnWindowResize(void* props) {}
		virtual void OnMainThreadProcess(void* task) {}
		virtual void OnTranslate() {}
	public:

		std::vector<int> m_RegisteredEvents;

		void HandleEvent(Event* e);

		virtual ~Listener() {}
	};

}

#endif // LISTENER_H_