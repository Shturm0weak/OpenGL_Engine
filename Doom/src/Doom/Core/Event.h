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
		const char* eventId;
		Listener* sender;

		Event(const char* eventId, Listener* sender, void* parameter = 0) {
			this->eventId = eventId;
			this->parameter = parameter;
			this->sender = sender;
		}

		~Event() {}

		inline const char* EventId() const {
			if (eventId != nullptr)
				return eventId;
			return nullptr;
		}
		inline void* Parameter() { return parameter; }
		inline Listener* Sender() { return sender; }
	};

}

#endif // EVENT_H_