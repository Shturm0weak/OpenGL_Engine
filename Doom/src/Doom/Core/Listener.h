#pragma once
#ifndef LISTENER_H_
#define LISTENER_H_
#include <iostream>
#include "Event.h"
#include <string>
#include "ThreadPool.h"
#include "Core.h"

namespace Doom {

	class DOOM_API Listener {
	private:
		virtual void OnUpdate() {}
		virtual void OnMove() {}
		virtual void OnRotate() {}
		virtual void OnStart() {}
		virtual void OnScale() {}
		virtual void OnCollision(void* col) {}
		virtual void OnWindowResize(void* props) {}
		virtual void OnMiss() {}
		virtual void OnMainThreadProcess(void* task) {}

	public:

		void HandleEvent(Event* e) {
			std::string s = e->EventId();
			if (s == "OnUpdate")
				OnUpdate();
			else if (s == "OnStart")
				OnStart();
			else if (s == "OnCollision") {
				int i = 1;
				e->Sender()->OnCollision(e->Parameter());
			}
			else if (s == "OnMove")
				e->Sender()->OnMove();
			else if (s == "OnRotate")
				e->Sender()->OnRotate();
			else if (s == "OnScale")
				e->Sender()->OnScale();
			else if (s == "OnWindowResize")
				OnWindowResize(e->Parameter());
			else if (s == "OnMiss")
				OnMiss();
			else if (s == "OnMainThreadProcess")
				OnMainThreadProcess(e->Parameter());
		}
	};

}

#endif // LISTENER_H_