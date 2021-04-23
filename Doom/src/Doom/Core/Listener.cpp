#include "Listener.h"
#include "EventSystem.h"

using namespace Doom;

void Listener::HandleEvent(Event* e)
{
	EventType type = (EventType)e->m_EventId;
	if (type == EventType::ONUPDATE)
	{
		OnUpdate();
	}
	else if (type == EventType::ONSTART)
	{
		OnStart();
	}
	else if (type == EventType::ONCOLLISION)
	{
		if (e->m_Parameter != nullptr)
		{
			e->m_Sender->OnCollision(e->m_Parameter);
		}
	}
	else if (type == EventType::ONMOVE)
	{
		e->m_Sender->OnMove();
	}
	else if (type == EventType::ONROTATE)
	{
		e->m_Sender->OnRotate();
	}
	else if (type == EventType::ONSCALE)
	{
		e->m_Sender->OnScale();
	}
	else if (type == EventType::ONTRANSLATE)
	{
		e->m_Sender->OnTranslate();
	}
	else if (type == EventType::ONWINDOWRESIZE)
	{
		OnWindowResize(e->m_Parameter);
	}
	else if (type == EventType::ONMAINTHREADPROCESS)
	{
		OnMainThreadProcess(e->m_Parameter);
	}
}