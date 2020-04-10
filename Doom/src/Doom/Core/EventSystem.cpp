#include "EventSystem.h"
#include <memory>

using namespace Doom;

bool EventSystem::AlreadyRegistered(const char* eventId, Listener* client) {

	bool alreadyRegistered = false;

	pair<multimap<const char*, Listener*>::iterator,
		multimap<const char*, Listener*>::iterator> range;

	range = database.equal_range(eventId);

	
	for (multimap<const char*, Listener*>::iterator iter = range.first;
		iter != range.second; iter++) {
		if ((*iter).second == client) {
			alreadyRegistered = true;
			break;
		}
	}

	return alreadyRegistered;
}

void EventSystem::DispatchEvent(Event* _event) {
	range = database.equal_range(_event->EventId());
	for (iter = range.first;iter != range.second; iter++) {
		(*iter).second->HandleEvent(_event);
		if ((std::string)_event->EventId() != "OnUpdate") {
			return;
		}
	}	
}

EventSystem* EventSystem::Instance() {
	static EventSystem instance;
	return &instance;
}

void EventSystem::RegisterClient(const char* event, Listener* client) {
	if (!client || AlreadyRegistered(event, client)) {
		return;
	}

	database.insert(std::make_pair(event, client));
}

void EventSystem::UnregisterClient(const char* event, Listener* client) {
	pair<multimap<const char*, Listener*>::iterator,
		multimap<const char*, Listener*>::iterator> range;

	range = database.equal_range(event);

	for (multimap<const char*, Listener*>::iterator iter = range.first;
		iter != range.second; iter++) {
		if ((*iter).second == client) {
			iter = database.erase(iter);
			break;
		}
	}
}

void EventSystem::UnregisterAll(Listener* client) {
	if (database.size() > 0) {
		multimap<const char*, Listener*>::iterator iter = database.begin();
		while (iter != database.end()) {
			if ((*iter).second == client) {
				iter = database.erase(iter);
			}
			else {
				iter++;
			}
		}
	}
}

void EventSystem::SendEvent(const char* eventId, Listener* sender, void* data)
{
	std::string s = eventId;
	if (s != "OnWindowResize" && s != "OnMainThreadProcess" && process_events == false)
		return;
	mtx1.lock();
	std::unique_ptr<Event> newEvent(new Event(eventId, sender, data));
	currentEvents.push(*newEvent);
	mtx1.unlock();
}

void EventSystem::ProcessEvents() {
	if (process_events == false)
		return;
	std::unique_lock<std::mutex> lck(mtx);
	while (currentEvents.size() > 0) {
		Event newEvent = currentEvents.front();
		currentEvents.pop();
		DispatchEvent(&newEvent);
	}
	lck.unlock();
}

void EventSystem::StopProcessEvents(bool value) {
	process_events = !value;
}

void EventSystem::ClearEvents() {
	while (currentEvents.size() > 0) {
		currentEvents.pop();
	}
}

void EventSystem::Shutdown() {
	database.clear();
	ClearEvents();
}