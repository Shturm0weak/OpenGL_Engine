#pragma once

#include <list>
#include <map>

using std::multimap;
using std::list;
using std::pair;

#include "Event.h"
#include <iostream>
#include "ThreadPool.h"
#include "Core.h"
#include "Listener.h"

namespace Doom {

	enum DOOM_API EventType {
		ONUPDATE,
		ONSTART,
		ONWINDOWRESIZE,
		ONMAINTHREADPROCESS,
		ONCOLLISION,
		ONMOVE,
		ONROTATE,
		ONSCALE,
		ONTRANSLATE
	};

	class DOOM_API EventSystem {
	private:

		multimap<EventType, Listener*> m_Database;

		pair<multimap<EventType, Listener*>::iterator,
			multimap<EventType, Listener*>::iterator> m_Range;

		multimap<EventType, Listener*>::iterator m_Iter;

		std::queue<Event> m_CurrentEvents;
		mutable std::mutex m_Mtx, m_Mtx1;
		std::condition_variable m_CondVar;
		bool m_IsProcessingEvents = true;

		void DispatchEvent(Event* event);

		EventSystem& operator=(const EventSystem& rhs) { return *this; }

		EventSystem(const EventSystem&) = delete;
		EventSystem() {}
	public:

		int GetAmountOfEvents();
		bool AlreadyRegistered(EventType eventId, Listener* client);
		static EventSystem& GetInstance();
		void StopProcessEvents(bool value);
		void RegisterClient(EventType event, Listener* client);
		void UnregisterClient(EventType event, Listener* client);
		void UnregisterAll(Listener* client);
		void SendEvent(EventType eventId, Listener* sender, void* data = 0);
		void ProcessEvents();
		void ClearEvents();
		void Shutdown();
	};

	class DOOM_API MainThread : Listener {
	private:

		Task m_Task;
	public:
		
		MainThread() 
		{
			EventSystem::GetInstance().RegisterClient(EventType::ONMAINTHREADPROCESS, (Listener*)this);
		}

		static MainThread& GetInstance()
		{
			static MainThread instance; return instance;
		}

		virtual void OnMainThreadProcess(void* task) override
		{
			m_Task = *static_cast<Task*>(task);
			m_Task();
			delete task;
		}
	};

}