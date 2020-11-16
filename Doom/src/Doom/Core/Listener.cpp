#include "Listener.h"
#include "EventSystem.h"

using namespace Doom;

void Listener::HandleEvent(Event* e) {
	EventType type = (EventType)e->GetEventId();
	if (type == EventType::ONUPDATE)
		OnUpdate();
	else if (type == EventType::ONSTART)
		OnStart();
	else if (type == EventType::ONCOLLSION)
		e->GetSender()->OnCollision(e->GetParameter());
	else if (type == EventType::ONMOVE)
		e->GetSender()->OnMove();
	else if (type == EventType::ONROTATE)
		e->GetSender()->OnRotate();
	else if (type == EventType::ONSCALE)
		e->GetSender()->OnScale();
	else if (type == EventType::ONTRANSLATE)
		e->GetSender()->OnTranslate();
	else if (type == EventType::ONWINDOWRESIZE)
		OnWindowResize(e->GetParameter());
	else if (type == EventType::ONMISS)
		OnMiss();
	else if (type == EventType::ONMAINTHREADPROCESS)
		OnMainThreadProcess(e->GetParameter());
}