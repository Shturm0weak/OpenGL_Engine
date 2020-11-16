#pragma once

#ifndef EVENT_H_
#define EVENT_H_
#include <functional>
#include "Core.h"

namespace Doom {

	class Listener;

	class DOOM_API Event {
	private:

		int eventId;
		Listener* sender;
		void* parameter;

	public:

		Event(int eventId, Listener* sender, void* parameter = 0) {
			this->eventId = eventId;
			this->parameter = parameter;
			this->sender = sender;
		}

		~Event() {}

		inline int GetEventId() {return eventId;}
		inline void* GetParameter() { return parameter; }
		inline Listener* GetSender() { return sender; }
	};

}

#endif // EVENT_H_