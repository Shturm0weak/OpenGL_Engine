#include "EventSystem.h"
#include <memory>

using namespace Doom;

int Doom::EventSystem::GetAmountOfEvents()
{
	return m_CurrentEvents.size();
}

bool EventSystem::AlreadyRegistered(EventType eventId, Listener* client) 
{

	bool alreadyRegistered = false;

	pair<multimap<EventType, Listener*>::iterator,
		multimap<EventType, Listener*>::iterator> range;

	range = m_Database.equal_range(eventId);

	
	for (multimap<EventType, Listener*>::iterator iter = range.first;
		iter != range.second; iter++)
	{
		if ((*iter).second == client) 
		{
			alreadyRegistered = true;
			break;
		}
	}

	return alreadyRegistered;
}

void EventSystem::DispatchEvent(Event* _event) 
{
	m_Range = m_Database.equal_range((EventType)_event->GetEventId());
	for (m_Iter = m_Range.first;m_Iter != m_Range.second; m_Iter++)
	{
		(*m_Iter).second->HandleEvent(_event);
		if (_event->GetEventId() != EventType::ONUPDATE)
		{
			return;
		}
	}	
}

EventSystem& EventSystem::GetInstance() 
{
	static EventSystem instance;
	return instance;
}

void EventSystem::RegisterClient(EventType event, Listener* client)
{
	if (!client || AlreadyRegistered(event, client)) return;

	client->m_RegisteredEvents.push_back((int)event);
	m_Database.insert(std::make_pair(event, client));
}

void EventSystem::UnregisterClient(EventType event, Listener* client) 
{
	pair<multimap<EventType, Listener*>::iterator,
		multimap<EventType, Listener*>::iterator> range;

	range = m_Database.equal_range(event);

	for (multimap<EventType, Listener*>::iterator iter = range.first;
		iter != range.second; iter++) 
	{
		if ((*iter).second == client) 
		{
			iter = m_Database.erase(iter);
			break;
		}
	}
}

void EventSystem::UnregisterAll(Listener* client) 
{
	if (m_Database.size() > 0) {
		multimap<EventType, Listener*>::iterator iter = m_Database.begin();
		while (iter != m_Database.end()) 
		{
			if ((*iter).second == client) iter = m_Database.erase(iter);
			else iter++;
		}
	}
}

void EventSystem::SendEvent(EventType eventId, Listener* sender, void* data)
{
	if (eventId != EventType::ONWINDOWRESIZE
		&& eventId != EventType::ONMAINTHREADPROCESS
		&& m_IsProcessingEvents == false) return;
	m_Mtx1.lock();
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
	m_CurrentEvents.push(*newEvent);
	m_Mtx1.unlock();
}

void EventSystem::ProcessEvents() 
{
	if (m_IsProcessingEvents == false) return;
	std::unique_lock<std::mutex> lck(m_Mtx);
	while (m_CurrentEvents.size() > 0) 
	{
		Event newEvent = m_CurrentEvents.front();
		m_CurrentEvents.pop();
		DispatchEvent(&newEvent);
	}
	lck.unlock();
}

void EventSystem::StopProcessEvents(bool value)
{
	m_IsProcessingEvents = !value;
}

void EventSystem::ClearEvents() 
{
	while (m_CurrentEvents.size() > 0)
	{
		m_CurrentEvents.pop();
	}
}

void EventSystem::Shutdown() 
{
	m_Database.clear();
	ClearEvents();
}