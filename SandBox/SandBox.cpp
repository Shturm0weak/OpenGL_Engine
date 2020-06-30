#include "Application.h"
#include "EntryPoint.h"
#include "src/game/Game.h"
#include "src/game/DrawLines.h"
#include "src/game/RayCastTest.h"
#include "src/game/TicTacToe.h"
#include "src/2Dshootergame/ShooterGame.h"
#include "src/game/StressTest.h"
#include "src/game/TestImGui.h"
#include "src/SpaceFire/SpaceFire.h"

int main() {
	StressTest* app = new StressTest();
	Doom::EntryPoint entrypoint(app);
	entrypoint.Run();
	return 0;
}