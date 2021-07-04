#include "EntryPoint.h"
#include "Audio/SoundManager.h"
#include "src/Examples/RayCastTest.h"
#include "src/Examples/TicTacToe.h"
#include "src/Examples/StressTest.h"
#include "src/SpaceFire/SpaceFire.h"
#include "src/Examples/BlankProject.h"
#include "FEM/FEM.h"
#include "AimTrainer/AimTrainer.h"
#include "Examples/TestCollision2D.h"
#include "Examples/StressTest2D.h"
#include "Hexagon/Hexagon.h"
#include "Examples/Test.h"
#include "Examples/Chess/Chess.h"
#include "Examples/Tetris/Tetris.h"

int main()
{
	Test app;
	Doom::EntryPoint entrypoint(nullptr);
	entrypoint.Run();
	return 0;
}