#include "Listener.h"
#include "EventSystem.h"

using namespace Doom;

void Listener::HandleEvent(Event* e) {
	EventType type = (EventType)e->EventId();
	if (type == EventType::ONUPDATE)
		OnUpdate();
	else if (type == EventType::ONSTART)
		OnStart();
	else if (type == EventType::ONCOLLSION)
		e->Sender()->OnCollision(e->Parameter());
	else if (type == EventType::ONMOVE)
		e->Sender()->OnMove();
	else if (type == EventType::ONROTATE)
		e->Sender()->OnRotate();
	else if (type == EventType::ONSCALE)
		e->Sender()->OnScale();
	else if (type == EventType::ONTRANSLATE)
		e->Sender()->OnTranslate();
	else if (type == EventType::ONWINDOWRESIZE)
		OnWindowResize(e->Parameter());
	else if (type == EventType::ONMISS)
		OnMiss();
	else if (type == EventType::ONMAINTHREADPROCESS)
		OnMainThreadProcess(e->Parameter());
}