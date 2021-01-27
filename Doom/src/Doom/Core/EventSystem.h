#pragma once

#ifndef _EVENTSYSTEM_H_
#define _EVENTSYSTEM_H_
#include <list>
#include <map>

using std::multimap;
using std::list;
using std::pair;

#include "Event.h"
#include "Listener.h"
#include <iostream>
#include "ThreadPool.h"
#include "Core.h"

namespace Doom {

	enum DOOM_API EventType {
		ONUPDATE,
		ONSTART,
		ONWINDOWRESIZE,
		ONMAINTHREADPROCESS,
		ONCOLLISION,
		ONMISS,
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

		EventSystem() {}
		
	public:

		EventSystem(EventSystem& rhs) {}
		~EventSystem() { this->Shutdown(); }

		int GetAmountOfEvents() { return m_CurrentEvents.size(); }
		bool AlreadyRegistered(EventType eventId, Listener* client);
		static EventSystem* GetInstance();
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
		
		MainThread() {
			EventSystem::GetInstance()->RegisterClient(EventType::ONMAINTHREADPROCESS, (Listener*)this);
		}

		static void Init() {
			static MainThread mainthread;
		}

		static MainThread* GetInstance() {
			static MainThread mainthread; return &mainthread;
		}

		virtual void OnMainThreadProcess(void* _task) override {
			m_Task = *static_cast<Task*>(_task);
			m_Task();
			delete _task;
		}
	};

}

#endif // _EVENTSYSTEM_H_