#include "EventSystem.h"
#include <memory>

using namespace Doom;

bool EventSystem::AlreadyRegistered(EventType eventId, Listener* client) {

	bool alreadyRegistered = false;

	pair<multimap<EventType, Listener*>::iterator,
		multimap<EventType, Listener*>::iterator> range;

	range = database.equal_range(eventId);

	
	for (multimap<EventType, Listener*>::iterator iter = range.first;
		iter != range.second; iter++) {
		if ((*iter).second == client) {
			alreadyRegistered = true;
			break;
		}
	}

	return alreadyRegistered;
}

void EventSystem::DispatchEvent(Event* _event) {
	range = database.equal_range((EventType)_event->eventId);
	for (iter = range.first;iter != range.second; iter++) {
		(*iter).second->HandleEvent(_event);
		if (_event->EventId() != EventType::ONUPDATE) {
			return;
		}
	}	
}

EventSystem* EventSystem::GetInstance() {
	static EventSystem instance;
	return &instance;
}

void EventSystem::RegisterClient(EventType event, Listener* client) {
	if (!client || AlreadyRegistered(event, client)) {
		return;
	}

	client->registeredEvents.push_back((int)event);
	database.insert(std::make_pair(event, client));
}

void EventSystem::UnregisterClient(EventType event, Listener* client) {
	pair<multimap<EventType, Listener*>::iterator,
		multimap<EventType, Listener*>::iterator> range;

	range = database.equal_range(event);

	for (multimap<EventType, Listener*>::iterator iter = range.first;
		iter != range.second; iter++) {
		if ((*iter).second == client) {
			iter = database.erase(iter);
			break;
		}
	}
}

void EventSystem::UnregisterAll(Listener* client) {
	if (database.size() > 0) {
		multimap<EventType, Listener*>::iterator iter = database.begin();
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

void EventSystem::SendEvent(EventType eventId, Listener* sender, void* data)
{
	if (eventId != EventType::ONWINDOWRESIZE && eventId != EventType::ONMAINTHREADPROCESS && process_events == false)
		return;
	mtx1.lock();
	/*for (auto i = database.begin(); i != database.end(); i++)
	{
		if (i->first == eventId) {
			std::unique_ptr<Event> newEvent(new Event(i->first, sender, data));
			currentEvents.push(*newEvent);
			mtx1.unlock();
			return;
		}
	}*/
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