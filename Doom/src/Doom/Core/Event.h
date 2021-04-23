#pragma once

#include <functional>
#include "Core.h"

namespace Doom {

	class Listener;

	class DOOM_API Event {
	public:

		Listener* m_Sender = nullptr;
		void* m_Parameter = nullptr;
		int m_EventId = -1;

		Event(int eventId, Listener* sender, void* parameter = 0)
			: m_EventId(eventId), m_Parameter(parameter), m_Sender(sender) {}

		~Event() {}
	};

}