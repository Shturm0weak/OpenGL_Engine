#include "Application.h"
#include "EntryPoint.h"
#include <stdio.h>
#include "src/game/Game.h"
#include "src/game/DrawLines.h"
#include "src/game/RayCastTest.h"
#include "src/game/TicTacToe.h"

int main() {
	TicTacToe* app = new TicTacToe();
	Doom::EntryPoint* entrypoint = new Doom::EntryPoint(app);
	entrypoint->Run();
	return 0;
}