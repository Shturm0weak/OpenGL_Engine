#include "Application.h"
#include "EntryPoint.h"
#include <stdio.h>
#include "src/game/Game.h"
#include "src/game/DrawLines.h"
#include "src/game/RayCastTest.h"

int main() {
	RayCastTest* app = new RayCastTest();
	Doom::EntryPoint* entrypoint = new Doom::EntryPoint(app);
	entrypoint->Run();
	return 0;
}