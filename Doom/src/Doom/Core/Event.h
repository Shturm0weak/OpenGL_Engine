#pragma once
#ifndef EVENT_H_
#define EVENT_H_
#include <functional>
#include "Core.h"

namespace Doom {

	class Listener;

	class DOOM_API Event {
	private:

		Listener* m_Sender;
		void* m_Parameter;
		int m_EventId;

	public:

		Event(int eventId, Listener* sender, void* parameter = 0) {
			this->m_EventId = eventId;
			this->m_Parameter = parameter;
			this->m_Sender = sender;
		}

		~Event() {}

		inline int GetEventId() {return m_EventId;}
		inline void* GetParameter() { return m_Parameter; }
		inline Listener* GetSender() { return m_Sender; }
	};

}

#endif // EVENT_H_