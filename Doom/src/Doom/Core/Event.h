#ifndef EVENT_H_
#define EVENT_H_
#include <functional>
#include "Core.h"

namespace Doom {

	class Listener;

	class DOOM_API Event {
	private:

		
		void* parameter;

	public:
		int eventId;
		Listener* sender;

		Event(int eventId, Listener* sender, void* parameter = 0) {
			this->eventId = eventId;
			this->parameter = parameter;
			this->sender = sender;
		}

		~Event() {}

		inline int EventId() {return this->eventId;}
		inline void* Parameter() { return parameter; }
		inline Listener* Sender() { return sender; }
	};

}

#endif // EVENT_H_